var g_DemoFile: Uint8Array;
var g_MapFile: Uint8Array;

var Module = {
	print :
	    (function() { return function(text) { console.log(text); }; })(),
	printErr : function(text) {
		if (arguments.length > 1)
			text = Array.prototype.slice.call(arguments).join(' ');
		if (0) {
			// dump(text + '\n');
		}
	},
	canvas : (function() {
		var canvas = document.getElementById('canvas');
		return canvas;
	})(),
	arguments : []
};

function RemoveEls() {
	document.getElementById('DemoFileInput').remove();
	document.getElementById('DemoFileLoad').remove();
	document.getElementById('renderdemos').remove();
	document.getElementById('MapFileInput').remove();
	document.getElementById('MapFileLoad').remove();
	document.getElementById('rendermap').remove();
}

function GetMapFromURL(): {map: string;} {
	if (location.search === "")
		return {map : ""};
	let o = {map : ""};
	let nvPairs =
	    location.search.substring(1).replace(/\+/g, " ").split("&");
	nvPairs.forEach(function(pair) {
		var e = pair.indexOf('=');
		var n = decodeURIComponent(e < 0 ? pair : pair.substring(0, e)),
		    v = (e < 0 || e + 1 == pair.length)
			    ? null
			    : decodeURIComponent(
				  pair.substring(e + 1, pair.length));
		o[n] = v;
	});

	return o;
}

function OnLoad() {
	let MapName = GetMapFromURL();

	if (MapName.map != "") {
		RemoveEls();

		var MapReq = new XMLHttpRequest();
		MapReq.open("GET", MapName.map + ".map", true);
		MapReq.responseType = "arraybuffer";

		MapReq.onload = function(evt) {
			LoadMapFileImpl(
			    MapName.map,
			    (evt.target as XMLHttpRequest).response as
				ArrayBuffer,
			    0);
		};

		MapReq.send(null);
	}
}

function LoadDemoFile() {
	var DemoFile: File, fr: FileReader;

	if (typeof window.FileReader !== 'function') {
		console.log("File API not supported.");
		return;
	}

	let FInput =
	    document.getElementById('DemoFileInput') as HTMLInputElement;
	if (!FInput) {
		console.log("Smth went really wrong.");
		return;
	} else if (!FInput.files) {
		console.log("No files available, browser bug?.");
		return;
	} else if (!FInput.files[0]) {
		console.log("No files selected.");
	} else {
		DemoFile = FInput.files[0];
		fr = new FileReader();
		fr.onload = GotBinary;
		fr.readAsArrayBuffer(DemoFile);
	}

	function GotBinary() {
		Module['arguments'].push('-d');
		Module['arguments'].push(DemoFile.name);
		g_DemoFile = new Uint8Array(fr.result as ArrayBuffer);
		Module['arguments'].push(g_DemoFile.byteLength.toString());

		RemoveEls();

		(document.getElementById('canvas') as HTMLCanvasElement)
		    .style.top = "0px";

		let DDNetJS =
		    document.createElement("script") as HTMLScriptElement;
		DDNetJS.src = "DDNet_demo.js";
		document.head.appendChild(DDNetJS);
	}
}

function LoadMapFileImpl(Name: string, Result: ArrayBuffer, RemEls: number) {
	Module['arguments'].push('-m');
	Module['arguments'].push(Name);
	g_MapFile = new Uint8Array(Result);
	Module['arguments'].push(g_MapFile.byteLength.toString());

	if (RemEls == 1)
		RemoveEls();

	(document.getElementById('canvas') as HTMLCanvasElement).style.top =
	    "0px";

	let DDNetJS = document.createElement("script") as HTMLScriptElement;
	DDNetJS.src = "DDNet_map.js";
	document.head.appendChild(DDNetJS);
}

function LoadMapFile() {
	var MapFile: File, fr: FileReader;

	if (typeof window.FileReader !== 'function') {
		console.log("File API not supported.");
		return;
	}

	let FInput =
	    document.getElementById('MapFileInput') as HTMLInputElement;
	if (!FInput) {
		console.log("Smth went really wrong.");
		return;
	} else if (!FInput.files) {
		console.log("No files available, browser bug?.");
		return;
	} else if (!FInput.files[0]) {
		console.log("No files selected.");
	} else {
		MapFile = FInput.files[0];
		fr = new FileReader();
		fr.onload = GotBinary;
		fr.readAsArrayBuffer(MapFile);
	}

	function GotBinary() {
		LoadMapFileImpl(MapFile.name, fr.result as ArrayBuffer, 1);
	}
}
