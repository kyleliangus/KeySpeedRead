var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// Construct URL
function foo(){
var url = 'http://api.nytimes.com/svc/topstories/v1/technology.json?api-key=138e6970de22bcc3a7cc412dad002b09:19:73186214';
xhrRequest(url, 'GET', 
  function(responseText)
	{
    // responseText contains a JSON object with article info
    var json = JSON.parse(responseText);
    var title = json.results[0].title;
    console.log('Title is ' + title);
    var abstract = json.results[0].abstract;      
    console.log('Abstract is ' + abstract);
            // Assemble dictionary using our keys
    var dictionary =
		{
      'KEY_TITLE' : title,
      'KEY_ABSTRACT' : abstract 
			
    };
      // Send to Pebble
    Pebble.sendAppMessage(dictionary,
      function(e) 
			{
        console.log('News info sent to Pebble successfully!');
      },
      function(e)
			{
        console.log('Error sending news info to Pebble!');
      }
    );
   }      
);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    foo();
  }                     
);