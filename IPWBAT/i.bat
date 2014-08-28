netsh wlan disconnect interface="ワイヤレス ネットワーク接続" 
netsh wlan delete profile name="NzXj AIBPXFJ" interface="ワイヤレス ネットワーク接続"
netsh interface set interface "ローカル エリア接続" ENABLED
reg add "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings" /f /v "ProxyEnable" /t REG_DWORD /d "1"
