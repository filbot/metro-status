var express = require('express');
var rp = require('request-promise');
var app = express();

// set the port of our application
// process.env.PORT lets the port be set by Heroku
var port = process.env.PORT || 8080;


function mean(numbers) {
  var total = 0;
  for (i = 0; i < numbers.length; i += 1) {
    total += numbers[i];
  }
  return Math.floor(total / numbers.length);
}

var options = {
  uri: 'http://api.metro.net/agencies/lametro-rail/stops/80136/predictions/',
  json: true // Automatically parses the JSON string in the response
};

app.get('/', function (req, res) {
  rp(options)
    .then(function (data) {
      let minutesArray = [];
      for (var i = 0; i < data.items.length; i++) {
        minutesArray.push(data.items[i].minutes);
      }
      average = mean(minutesArray);
      res.send(average.toString());
    })
    .catch(function (err) {
    });
});

app.listen(port, function() {
    console.log('Our app is running on http://localhost:' + port);
});
