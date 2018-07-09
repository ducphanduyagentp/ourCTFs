<?php
set_time_limit(20);
error_reporting(0);
require_once("flag.php");
echo "ENTER CODE: ";
$line = stream_get_line(STDIN, 8000, PHP_EOL);
echo PHP_EOL;

$str = "";
$crc32 = str_pad(dechex(crc32($line)), 8, '0', STR_PAD_LEFT);
$final = $crc32 . $line;

$chunks = chunk_split($final, 2, "|");
$explode = explode("|", $chunks);

foreach($explode as $chr){
	$str .= chr(hexdec($chr));
}
try {
	eval($str);
} catch (Throwable $t) {
	var_dump($final);
	var_dump($str);
	die("Unable to run code".PHP_EOL);
}

