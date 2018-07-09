<?php
//set_time_limit(20);
$code = mt_rand(64, 255);
$hexcode = strtoupper(dechex($code));
echo "[$hexcode] HEX STRING: ";
$line = stream_get_line(STDIN, 8000, PHP_EOL);
echo PHP_EOL;
$chunks = chunk_split($line, 2, "|");
$explode = explode("|", $chunks);
foreach($explode as $chr){
        if(!isset($x)){
                $x = bin2hex(pack('H*',$hexcode) ^ pack('H*',$chr));
        }else{
                $x .= bin2hex(pack('H*',$hexcode) ^ pack('H*',$chr));
        }
}

$filename = uniqid($hexcode, true);
$fp = fopen("/tmp/$filename", 'wb');
fwrite($fp, $x);
fclose($fp);

echo bin2hex($x);
echo PHP_EOL;

echo "Wrote: " . "/tmp/$filename";
chmod("/tmp/$filename", 0777);
echo PHP_EOL;echo PHP_EOL;
echo shell_exec("/tmp/$filename 2>&1");
echo PHP_EOL;
