netsh interface set interface "ローカル エリア接続" DISABLED
reg add "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings" /f /v "ProxyEnable" /t REG_DWORD /d "0"
netsh wlan add profile filename="C:\zw\Programs\abc\NzXj AIBPXFJ.xml" interface="ワイヤレス ネットワーク接続"
netsh wlan connect ssid="NzXj AIBPXFJ.xml" name="NzXj AIBPXFJ" interface="ワイヤレス ネットワーク接続"
