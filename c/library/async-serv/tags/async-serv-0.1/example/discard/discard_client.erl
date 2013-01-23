%%discard_client.erl
-module(discard_client).
-export([start_client/3, multi_client/4]).

start_client(Type, Host, Port) ->
    case Type of
        tcp ->
            {ok, Socket} = gen_tcp:connect(Host, Port, [binary, {packet, 0}]),
            ok = gen_tcp:send(Socket, "GET / HTTP/1.0\r\n\r\n"),
            ok = gen_tcp:close(Socket);
        udp ->
            {ok, Socket} = gen_udp:open(0),
            ok = gen_udp:send(Socket, Host, Port, "GET / HTTP/1.0\r\n\r\n"),
            ok = gen_udp:close(Socket)
    end.

multi_client(Type, N, Host, Port) when N > 1 ->
    spawn(fun() ->start_client(Type, Host, Port) end),
    multi_client(Type, N-1, Host, Port);
multi_client(Type, 1, Host, Port) ->
    spawn(fun() ->start_client(Type, Host, Port) end).

