
generate uuid in power shell
```powershell
$string = "test"; $hash = [System.Security.Cryptography.SHA256]::Create().ComputeHash([System.Text.Encoding]::UTF8.GetBytes($string)); [Guid]::New([byte[]]($hash[0..15]))
```