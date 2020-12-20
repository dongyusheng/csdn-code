package conf

type AppConf struct {
	KafkaConf `ini:"kafka"`
	TailLogConf `ini:"taillog"`
}

// kafka配置
type KafkaConf struct {
	Address string `ini:"address"`
	Topic string `ini:"topic"`
}


// taillog配置
type TailLogConf struct {
	FileName string `ini:"filename"`
}