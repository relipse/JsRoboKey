Welcome
-------
Welcome, JsRoboKey was inspired by AutoHotKey, Qt, Cross-platform coding, and javascript (thanks nodejs for making javascript real)
Think of desktop scripting using javascript.

Example Scripts
---------------
A simple script might look like this:
```
    //run notepad, wait a little bit for it to open, then send some keystrokes
    rk.run('notepad');
    rk.sleep(700);
    rk.sendKeys('abcdefgABCDEFG1234567890!@#$%^&*()');
```
Or even better, trigger to run notepad after a certain hotkey!
```
rk.addGlobalHotkey('Meta+Alt+N', function(){
   rk.run('notepad');
});
```
(Meta) means the Windows key

And demonstrating some new functionality, this script will run notepad and wait for it to open
```
function fgWinMatches(winTitle){
    var hwnd = rk.getForegroundWindow();
    var title = rk.getWindowText(hwnd);
    if (title.toLowerCase().indexOf(winTitle.toLowerCase()) >= 0){
       return {hwnd:hwnd,title:title};
    }else{
       return false;
    }
}

function winWait(winTitle, callback){
   var match = fgWinMatches(winTitle);
   if (match === false){
      rk.setTimeout(function(){
         winWait(winTitle, callback);
      },400);
   }else{
     callback(match);
   }
}

rk.run('notepad');
winWait('notepad', function(match){
    rk.sendKeys('Hello World');
});
```

A new feature onClipboardChange() check it out!!!
```
rk.onClipboardChange(function(data){
    if (typeof(data) != 'string'){ return; }
    if (data.indexOf('foobar') >= 0){
       rk.alert('foobar on clipboard: ' + data);
    }
});
```

Need Helpers
------------
In order to make this a reality, I am going to need some passionate helpers to get this working, we need most all of the features which AutoHotkey has, and cross-platform.
The features are so many so just start coding, shoot me an email at relipse@gmail.com and start by forking the repo. Make a few changes, fix a few bugs or add a few features!
Then submit a pull request!

Goal
----
To get a usable desktop scripting library that works cross-platform, windows, linux, and mac.


Compilation
-----------
First, you'll need >= Qt 5.1, then you'll need to follow these instructions here: http://qt-project.org/wiki/LibQxt_in_QtCreator
in order to get libqxt working for Qt 5

After that, you'll have to make sure the .pro file has the correct paths to the lib files.
Then it should complie just fine!

License
-------
Everything here is GPL 3 to protect it from becoming proprietary.


Tributes
--------
These fellows are inspiration
AutoHotkey - http://www.autohotkey.com/ and https://github.com/Lexikos/AutoHotkey_L
Qt - http://qt-project.org


