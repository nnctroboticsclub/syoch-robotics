# VPN Commands

# [VPN] Deploy Ws-relay
vdw:
	scp ws-relay/main.py \
			ws-relay/log-reader.py \
			ws-relay/tag-lister.py \
			ws-relay/requirements.txt \
			$(VPN_SSH):/home/syoch/ws-relay/

# [VPN] ForWarD
vfwd:
	ssh \
		-gL 8011:$(REMOTE_ESP32_IP):80 \
		-gL 8012:localhost:8000 \
		$(VPN_SSH)

# [VPN] Reverse ForWarD
vrfwd:
	ssh \
		-gR 0.0.0.0:8050:localhost:8080 \
		$(VPN_SSH)

# [VPN] Start SSH Connection
vpn:
	ssh $(VPN_SSH)