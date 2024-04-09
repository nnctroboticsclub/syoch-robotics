ws:
	watch -n 1 ls /mnt/st*

lc:
	wc -l \
		$$(find ./stm32-main/src -type f)

