Pebble.addEventListener('ready', function(e) {
	//...
});


Pebble.addEventListener("showConfiguration", function(e) {
	//todo: finish config screen launch
	
	var watchinfo;
	
	if (Pebble.getActiveWatchInfo) {
		watchinfo = Pebble.getActiveWatchInfo(); 
	} else {
		watchinfo = {};
	}
	
	//Pebble.openURL('http://theksmith.com/dev/pebble-config.php' + '?platform=' + watchinfo.platform + '&account=' + Pebble.getAccountToken());
});


Pebble.addEventListener("webviewclosed", function(e) {
    //...
});