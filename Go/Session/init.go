package session

import (
	"fmt"
)

var (
	sessionMgr SessionMgr
)

func Init(provider, addr string, options ...string) (err error) {
	switch provider {
	case "memory":
		sessionMgr = NewMemorySeesionMgr()
	case "redis":
		sessionMgr = NewRedisSeesionMgr()
	default:
		fmt.Errorf("不支持的存储类型")
		return
	}

	err = sessionMgr.Init(addr, options...)
	return
}
