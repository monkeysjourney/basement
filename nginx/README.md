nginx.conf		通过nginx串联的方式插入广告 openwrt中nginx的配置文件
				需要配置： iptables -t nat -A PREROUTING -s 0.0.0.0/0.0.0.0 -p tcp --dport 80 -j REDIRECT --to-ports 80
portal.conf		反向代理portal服务器
tornado.conf	tornado框架的代理配置
