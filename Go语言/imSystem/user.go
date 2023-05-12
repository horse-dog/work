package main

import (
	"net"
	"strings"
)

type User struct {
	Name   string
	Addr   string
	pipe   chan string
	conn   net.Conn
	server *Server
}

func NewUser(conn net.Conn, server *Server) *User {
	userAddr := conn.RemoteAddr().String()

	user := &User{
		Name:   userAddr,
		Addr:   userAddr,
		pipe:   make(chan string),
		conn:   conn,
		server: server,
	}

	go user.ListenMessage()

	return user
}

func (user *User) ListenMessage() {
	for {
		msg := <-user.pipe
		user.conn.Write([]byte(msg + "\n"))
	}
}

// 用户上线
func (user *User) Online() {

	user.server.mapLock.Lock()
	user.server.OnlineMap[user.Name] = user
	user.server.mapLock.Unlock()

	user.server.BroadCast(user, "已上线")
}

// 用户下线
func (user *User) Offline() {
	user.server.mapLock.Lock()
	delete(user.server.OnlineMap, user.Name)
	user.server.mapLock.Unlock()

	user.server.BroadCast(user, "下线")
}

// 处理消息
func (user *User) DoMessage(msg string) {
	if msg == "who" {
		user.server.mapLock.Lock()
		for _, usr := range user.server.OnlineMap {
			onlineMsg := "[" + usr.Addr + "]" + usr.Name + ":" + "在线...\n"
			user.SendMsg(onlineMsg)
		}
		user.server.mapLock.Unlock()
	} else if len(msg) > 7 && msg[:7] == "rename|" {
		newName := strings.Split(msg, "|")[1]
		_, ok := user.server.OnlineMap[newName]
		if ok {
			user.SendMsg("当前用户名被使用\n")
		} else {
			user.server.mapLock.Lock()
			delete(user.server.OnlineMap, user.Name)
			user.server.OnlineMap[newName] = user
			user.server.mapLock.Unlock()
			user.Name = newName
			user.SendMsg("您已经更新用户名:" + user.Name + "\n")
		}
	} else if len(msg) > 4 && msg[:3] == "to|" {
		remoteName := strings.Split(msg, "|")[1]
		remoteUser, ok := user.server.OnlineMap[remoteName]
		if !ok {
			user.SendMsg("该用户名不存在\n")
			return
		}
		content := strings.Split(msg, "|")[2]
		if content == "" {
			user.SendMsg("无消息内容，请重发\n")
			return
		}
		remoteUser.SendMsg(user.Name + "对您说:" + content + "\n")
	} else {
		user.server.BroadCast(user, msg)
	}
}

// 给当前User对应的客户端发送消息
func (user *User) SendMsg(msg string) {
	user.conn.Write([]byte(msg))
}
