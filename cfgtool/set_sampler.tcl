#!/usr/bin/expect
if { $argc == 4 } {
	set routerip [lindex $argv 0]
	set interface [lindex $argv 1]
	set type [lindex $argv 2]
	set value [lindex $argv 3]
} else { puts "Usage: set_sampler.tcl routerip interface type value!\n" ; exit 1}

set timeout 5
set password 123

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "system-view\r"
expect "]"
send "interface GigabitEthernet 0/0/$interface\r"
expect "]"
send "undo ip netstream inbound\r"
expect "]"
send "undo ip netstream outbound\r"
expect "]"
send "ip netstream sampler $type $value inbound\r"
expect "]"
send "ip netstream sampler $type $value outbound\r"
expect "]"
send "ip netstream inbound\r"
expect "]"
send "ip netstream outbound\r"
expect "]"
send "quit\r"
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect "]"

