var http = require("http");
var formidable = require('formidable');
var util = require("util");
var fs = require("fs");
var sd = require("silly-datetime");
var path = require("path");
var url = require("url");
var iconv = require('iconv-lite');

var server = http.createServer(function (req, res) {
	
	if(req.url == "/ImgReg" && req.method.toLowerCase() == "post") {
		res.setHeader('Access-Control-Allow-Orgin', '*');
		var form = new formidable.IncomingForm();
		form.uploadDir = "./source";
		form.keepExtensions = true;

		form.parse(req, function(err, fields, files) {
			if (err) {
				throw err;
			}
			console.log(util.inspect({fields: fields, files: files}));

			var date = sd.format(new Date(), 'YYYYMMDDHHmmss');
			//拿到文件的扩展名
			var extname = path.extname(files.image.name);
			var newpath = __dirname + "/source/" + fields.account + date + extname;
			var oldpath =   path.normalize(files.image.path);
			console.log(newpath);


			fs.rename(oldpath, newpath, function(err) {
				if (err) {
						throw err;
					}

					response = {
                   			message:'File uploaded successfully', 
                   			filename:files.image.name            
              		};
              		console.log(response);

              		var child_process = require('child_process');
              		var RegProcess = child_process.spawn('python', ['./baidu_food_reg.py', newpath]);
              		RegProcess.stdout.on('data', function(data) {
              			console.log('stdout: ' + iconv.decode(data, 'gbk'));
              			res.writeHead(200, {'content-type': 'text/html'});
              			res.end(data);
              		});

            		RegProcess.stderr.on('data', function(data, data) {
            			console.log('stderr: ' + data);
            		});

            		RegProcess.on('exit', function(code) {
            			console.log('exit_code: ' + code);
            		});	
			});

			/*fs.readFile(files.image.path, function(err, data) {
				fs.writeFile(filepath, data, function(err) {
					if (err) {
						throw err;
					}
					response = {
                   			message:'File uploaded successfully', 
                   			filename:files.image.name
              		};
              		console.log(response);
              		res.end(JSON.stringify(response));
				})
			})*/

		});
	} 
	else if(req.url == "/")
	{
		res.setHeader('Access-Control-Allow-Origin', '*');
		fs.readFile("./index.html", function(err, data) {
			res.writeHead(200, {'content-type': 'text/html'});
			res.end(data);
		});
	}
});
server.listen(3000, "localhost");
console.log('Server running at http://127.0.0.1:3000/');
