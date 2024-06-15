#!/bin/sh
./tun-read | ssh vpn@vpn-server ./endpoint.sh | ./tun-write
