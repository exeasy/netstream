#!/usr/bin/expect
if { $argc == 2 } {
	set routerip [lindex $argv 0]
	set option [lindex $argv 1]
} else { puts "Usage: set_tcpflag.tcl routerip enable!\n" }

set timeout 5
set password 123

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "system-view\r"
expect "]"
if { $option == "enable" } { send "ip netstream tcp-flag enable\r" 
} else { send "undo ip netstream tcp-flag enable\r" }
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect "]"
