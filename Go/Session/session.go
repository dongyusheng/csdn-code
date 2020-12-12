package session

// Session ...
// 对Session的增删改查、存储, memory_session和redis_session会实现这个接口
type Session interface {
	Set(key string, value interface{}) error
	Get(key string) (interface{}, error)
	Del(key string) error
	Save() error
}
