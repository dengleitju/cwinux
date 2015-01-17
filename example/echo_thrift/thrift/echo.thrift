namespace cpp echo_thrift

struct EchoData{
  1: string           data
}

service Echo{
   EchoData Echo(1:string echo_data)
}
