<?php
/*
	Code by : Rawee Chaithip
	Github : eewar
	
	Send GET to this file, http://THIS_FILE.php?data=JSON_DATA
	Sample of JSON_DATA = '{"uid":"u0000","up":610320,"v":[29.98,85.97,56.27]}'
	[uid] and [v] is required
*/
date_default_timezone_set("ASIA/BANGKOK");
$ts = time();

$csv_based_folder = 'csv';
$csv_filename = date("Y-m-d", $ts).'.csv';
$csv_datetime = date("Y-m-d H:i:s", $ts);

if (!isset($_GET['data'])):
	exit('GET required<br>http://'.$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'].'?data={"uid":"u0000","up":610320,"v":[29.98,85.97,56.27]}');
endif;

$json = $_GET['data'];
$data = json_decode($json, true);

if (!isset($data['uid']) || !isset($data['v'])):
	exit('GET data is missing [uid] or [v]');
endif;

$sensor_id = $data['uid'];
$values['dt'] = $csv_datetime;
$values += $data['v'];

$csv_folder = $csv_based_folder.'/'.$sensor_id.'/';
if (!is_dir($csv_based_folder)) mkdir($csv_based_folder);
if (!is_dir($csv_folder)) mkdir($csv_folder);

$f = fopen($csv_folder.$csv_filename, 'a');
fputcsv($f, $values);
fclose($f);

echo "finished!";
?>