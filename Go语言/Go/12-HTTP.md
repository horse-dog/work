## 启动 http 服务

```go
import (
    "net/http"
)

func main() {
    http.HandleFunc("/ping", func(w http.ResponseWriter, r *http.Request) {
        w.Write([]byte("pong"))
    })

    http.ListenAndServe(":8091", nil)
}
```

## 发送 http 请求

```go
func main() {
    reqBody, _ := json.Marshal(map[string]string{"key1": "val1", "key2": "val2"})
    
    resp, _ := http.Post(":8091", "application/json", bytes.NewReader(reqBody))
    defer resp.Body.Close()
    
    respBody, _ := io.ReadAll(resp.Body)
    fmt.Printf("resp: %s", respBody)
}
```

## 服务端

- 核心数据结构

    ```go
    type Server struct {
        // server 的地址
        Addr string
        // 路由器.
        Handler Handler // handler to invoke, http.DefaultServeMux if nil
        // ...
    }

    type Handler interface {
        ServeHTTP(ResponseWriter, *Request)
    }

    type ServeMux struct {
        mu sync.RWMutex
        m map[string]muxEntry
        es []muxEntry // slice of entries sorted from longest to shortest.
        hosts bool // whether any patterns contain hostnames
    }

    type muxEntry struct {
        h Handler
        pattern string 
    }
    ```

- 注册 handler

    ```go
    var DefaultServeMux = &defaultServeMux

    var defaultServeMux ServeMux

    type HandlerFunc func(ResponseWriter, *Request)

    // ServeHTTP calls f(w, r).
    func (f HandlerFunc) ServeHTTP(w ResponseWriter, r *Request) {
        f(w, r)
    }

    func HandleFunc(pattern string, handler func(ResponseWriter, *Request)) {
        DefaultServeMux.HandleFunc(pattern, handler)
    }

    func (mux *ServeMux) HandleFunc(pattern string, handler func(ResponseWriter, *Request)) {
        // ...
        mux.Handle(pattern, HandlerFunc(handler))
    }

    func (mux *ServeMux) Handle(pattern string, handler Handler) {
        mux.mu.Lock()
        defer mux.mu.Unlock()
        // ...

        e := muxEntry{h: handler, pattern: pattern}
        mux.m[pattern] = e
        if pattern[len(pattern)-1] == '/' {
            mux.es = appendSorted(mux.es, e)
        }
        // ...
    }

    func appendSorted(es []muxEntry, e muxEntry) []muxEntry {
        n := len(es)
        i := sort.Search(n, func(i int) bool {
            return len(es[i].pattern) < len(e.pattern)
        })
        if i == n {
            return append(es, e)
        }
        es = append(es, muxEntry{}) // try to grow the slice in place, any entry works.
        copy(es[i+1:], es[i:])      // Move shorter entries down
        es[i] = e
        return es
    }
    ```

- 启动 server

```go
func ListenAndServe(addr string, handler Handler) error {
    server := &Server{Addr: addr, Handler: handler}
    return server.ListenAndServe()
}

func (srv *Server) ListenAndServe() error {
    // ...
    addr := srv.Addr
    if addr == "" {
        addr = ":http"
    }
    ln, err := net.Listen("tcp", addr)
    // ...    
    return srv.Serve(ln)
}

var ServerContextKey = &contextKey{"http-server"}

type contextKey struct {
    name string
}

func (srv *Server) Serve(l net.Listener) error {
   // ...
   ctx := context.WithValue(baseCtx, ServerContextKey, srv)
    for {
        rw, err := l.Accept()
        // ...
        connCtx := ctx
        // ...
        c := srv.newConn(rw)
        // ...
        go c.serve(connCtx)
    }
}

func (c *conn) serve(ctx context.Context) {
    // ...
    c.r = &connReader{conn: c}
    c.bufr = newBufioReader(c.r)
    c.bufw = newBufioWriterSize(checkConnErrorWriter{c}, 4<<10)

    for {
        w, err := c.readRequest(ctx)
        // ...
        serverHandler{c.server}.ServeHTTP(w, w.req)
        w.cancelCtx()
        // ...
    }
}

func (sh serverHandler) ServeHTTP(rw ResponseWriter, req *Request) {
    handler := sh.srv.Handler
    if handler == nil {
        handler = DefaultServeMux
    }
    // ...
    handler.ServeHTTP(rw, req)
}

func (mux *ServeMux) ServeHTTP(w ResponseWriter, r *Request) {
    // ...
    h, _ := mux.Handler(r)
    h.ServeHTTP(w, r)
}

func (mux *ServeMux) Handler(r *Request) (h Handler, pattern string) {
    // ...
    return mux.handler(host, r.URL.Path)
}

func (mux *ServeMux) handler(host, path string) (h Handler, pattern string) {
    mux.mu.RLock()
    defer mux.mu.RUnlock()
    
    // ...
    h, pattern = mux.match(path)
    // ...
    return
}

func (mux *ServeMux) match(path string) (h Handler, pattern string) {
    v, ok := mux.m[path]
    if ok {
        return v.h, v.pattern
    }

    // ServeMux.es 本身是按照 pattern 的长度由大到小排列的
    for _, e := range mux.es {
        if strings.HasPrefix(path, e.pattern) {
            return e.h, e.pattern
        }
    }
    return nil, ""
}
```

## 客户端

- 核心数据结构

    ```go
    type Client struct {
        // ...
        Transport RoundTripper
        // ...
        Jar CookieJar
        // ...
        Timeout time.Duration
    }

    type RoundTripper interface {
        RoundTrip(*Request) (*Response, error)
    }

    type Transport struct {
        idleConn     map[connectMethodKey][]*persistConn // most recently used at end
        // ...
        DialContext func(ctx context.Context, network, addr string) (net.Conn, error)
        // ...
    }

    type Request struct {
        // 方法
        Method string
        // 请求路径
        URL *url.URL
        // 请求头
        Header Header
        // 请求参数内容
        Body io.ReadCloser
        // 服务器主机
        Host string
        // query 请求参数
        Form url.Values
        // 响应参数 struct
        Response *Response
        // 请求链路的上下文
        ctx context.Context
        // ...
    }

    type Response struct {
        // 请求状态，200 为 请求成功
        StatusCode int    // e.g. 200
        // http 协议，如：HTTP/1.0
        Proto      string // e.g. "HTTP/1.0"
        // 请求头
        Header Header
        // 响应参数内容  
        Body io.ReadCloser
        // 指向请求参数
        Request *Request
        // ...
    }
    ```

    - Client.Transport：负责 http 通信的核心部分

    - Client.Jar：cookie 管理

    - Client.Timeout：超时设置

    - RoundTripper 是通信模块的 interface，需要实现方法 Roundtrip，即通过传入请求 Request，与服务端交互后获得响应 Response

    - Tranport 是 RoundTripper 的实现类，核心字段包括

        - idleConn：空闲连接 map，实现复用

        - DialContext：新连接生成器

- 方法链路总览

    - 构造 http 请求参数

    - 获取用于与服务端交互的 tcp 连接

    - 通过 tcp 连接发送请求参数

    - 通过 tcp 连接接收响应结果

- Client.Post

    - 调用 net/http 包下的公开方法 Post 时，需要传入服务端地址 url，请求参数格式 contentType 以及请求参数的 io reader. 方法中会使用包下的单例客户端 DefaultClient 处理这次请求.

    ```go
    var DefaultClient = &Client{}

    func Post(url, contentType string, body io.Reader) (resp *Response, err error) {
        return DefaultClient.Post(url, contentType, body)
    }

    func (c *Client) Post(url, contentType string, body io.Reader) (resp *Response, err error) {
        req, err := NewRequest("POST", url, body)
        // ...
        req.Header.Set("Content-Type", contentType)
        return c.Do(req)
    }

    func NewRequestWithContext(ctx context.Context, method, url string, body io.Reader) (*Request, error) {
        // ...
        u, err := urlpkg.Parse(url)
        // ...
        rc, ok := body.(io.ReadCloser)
        // ...
        req := &Request{
            ctx:        ctx,
            Method:     method,
            URL:        u,
            // ...
            Header:     make(Header),
            Body:       rc,
            Host:       u.Host,
        }
        // ...
        return req, nil
    }
    ```

- Client.Do

    - Client.send 方法中，会在通过 send 方法发送请求的前后，分别对 cookie 进行更新

    - 在调用 send 方法时，需要注入 RoundTripper 模块，默认会使用全局单例 DefaultTransport 进行注入，核心逻辑位于 Transport.RoundTrip 方法中，其中分为两个步骤

        - 获取/构造 tcp 连接

        - 通过 tcp 连接完成与服务端的交互

    ```go
    func (c *Client) Do(req *Request) (*Response, error) {
        return c.do(req)
    }

    func (c *Client) do(req *Request) (retres *Response, reterr error) {
        var (
            deadline      = c.deadline()
            resp          *Response
            // ...
        )    
        for {
            // ...
            var err error       
            if resp, didTimeout, err = c.send(req, deadline); err != nil {
                // ...
            }
            // ...
        }
    }

    func (c *Client) send(req *Request, deadline time.Time) (resp *Response, didTimeout func() bool, err error) {
        // 设置 cookie 到请求头中
        if c.Jar != nil {
            for _, cookie := range c.Jar.Cookies(req.URL) {
                req.AddCookie(cookie)
            }
        }
        // 发送请求
        resp, didTimeout, err = send(req, c.transport(), deadline)
        if err != nil {
            return nil, didTimeout, err
        }
        // 更新 resp 的 cookie 到请求头中
        if c.Jar != nil {
            if rc := resp.Cookies(); len(rc) > 0 {
                c.Jar.SetCookies(req.URL, rc)
            }
        }
        return resp, nil, nil
    }

    func (c *Client) transport() RoundTripper {
        if c.Transport != nil {
            return c.Transport
        }
        return DefaultTransport
    }

    func send(ireq *Request, rt RoundTripper, deadline time.Time) (resp *Response, didTimeout func() bool, err error) {
        // ...
        resp, err = rt.RoundTrip(req)
        // ...
        return resp, nil, nil
    }

    func (t *Transport) RoundTrip(req *Request) (*Response, error) {
        return t.roundTrip(req)
    }

    func (t *Transport) roundTrip(req *Request) (*Response, error) {
        // ...
        for {          
            // ...    
            treq := &transportRequest{Request: req, trace: trace, cancelKey: cancelKey}      
            // ...
            pconn, err := t.getConn(treq, cm)        
            // ...
            resp, err = pconn.roundTrip(treq)          
            // ...
        }
    }
    ```

- Transport.getConn

    - 通过 queueForIdleConn 方法，尝试复用采用相同协议、访问相同服务端地址的空闲连接

    - 倘若无可用连接，则通过 queueForDial 方法，异步创建一个新的连接，并通过接收 ready channel 信号的方式，确认构造连接的工作已经完成

    ```go
    func (t *Transport) getConn(treq *transportRequest, cm connectMethod) (pc *persistConn, err error) {
        // 获取连接的请求参数体
        w := &wantConn{
            cm:         cm,
            // key 由 http 协议、服务端地址等信息组成
            key:        cm.key(),
            ctx:        ctx,
            // 标识连接构造成功的信号发射器
            ready:      make(chan struct{}, 1),
        }
        // 倘若连接获取失败，在 wantConn.cancel 方法中，会尝试将 tcp 连接放回队列中以供后续复用
        defer func() {
            if err != nil {
                w.cancel(t, err)
            }
        }()
        // 尝试复用指向相同服务端地址的空闲连接
        if delivered := t.queueForIdleConn(w); delivered {
            pc := w.pc
            // ...
            return pc, nil
        }
        // 异步构造新的连接
        t.queueForDial(w)
        select {
        // 通过阻塞等待信号的方式，等待连接获取完成
        case <-w.ready:
            // ...
            return w.pc, w.err
        // ...
        }
    }
    ```