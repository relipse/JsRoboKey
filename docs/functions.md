Functions and Methods
---------------------
A variety of functions and methods exist.
The easiest way to see what is supported in your current version is to run rk.getMethods() or rk.help() in the console sandbox. (some of the ones listed you don't want to call)

But I will try to post what is current (0.0.0) here as well.

* clipboard() -- get the clipboard text
isMainScriptLoaded() -- check to see whether the main script is loaded (homePath/jsrobokey_main.js)
* fileExists(string file) -- check if a file exists
* require(string filename) -- require/include once a certain file (won't include unless already included)
* include(string filename) -- include/evaluate a js file
* addGlobalHotkey(string hotkeycombo, function callback) -- add a permanent global hotkey Meta means Win key, such as "Ctrl+Meta-N", callback is the function that gets called when the hotkey combo gets pressed
* onClipboardChange(function callback(data)) -- whenever the clipboard changes this function will be called with whatever data (text) is on it
* download(string url,function callback(data)) -- download a file from the internet and call callback with the data as a string returned
* openUrl(string url) -- open a url in the default browser
* openFile(string file) -- open a file with the default application
* open(string file) -- same as openUrl()
* run(file, array|string args|arg1) -- same as runSpawn() right now
* runSpawn(string file, array|string args|arg1) -- spawn an executable (do not wait for response) for example runSpawn('notepad', 'myfile'); would run notepad sending the parameter myfile to it
* string runWait(string file,array|string args|arg1) -- run an executable with given parameters and wait for and return the result (will halt script until finishes)
* array of strings getIncludedFiles() -- get a list of all the files and modules included (no duplicates)
* getLoadedModuleFileStack() -- see a list of all modules and files loaded in the order in which they were loaded (probably useful for debugging)
* alert(string msg [,string title]) -- show an alert box with a message and optional title can use without "rk."
* int setTimeout(function callback,int milliseconds) -- call a function after a certain number of milliseconds. returns a timeout id if you need to stop it or check up on it later
* timeoutRemainingTime(int timeoutId) -- check out how much time is left on the timeoutId
* clearTimeout(int) -- delete a timeout (doesn't do anything if already executed) stop/prevent it from executing the callback function
* sendKeys(string keys_to_send) -- emulate typing a bunch of keys for example rk.run('notepad'); rk.sleep(700); rk.sendKeys('hello I am in notepad');
* build() -- get the latest build number
* version() -- get the latest version
* sleep(int ms) -- sleep for a certain period of milliseconds 
* exit() -- exit JsRoboKey
* string compilationDate() -- get the date the version was compiled
* string help() -- return a little help message
* array of strings getMethods() -- show all the possible callable methods on the rk object
* int findWindow(QString,QString) -- find a window by title and/or class
* int getForegroundWindow() -- grab the handle (hwnd) of the window in the foreground
* string getWindowText(int hwnd) -- get the title or window text of the hwnd
* closeWindow(int hwnd) -- call a close window on the particular hwnd
* setTrayIcon(string file_such_as_a_png) -- change the tray icon of JsRoboKey (cool for custom scripts?) if the file is invalid the tray icon will be empty (but still there)
* trayMsg(string title[,string body[,function onclick_callback[,int iconType[,int ms_duration]) -- show a tray message with the title and body (body can contain basic html) specify a callback if you want to do something when/if the user clicks on the icon, iconType isn't really used right now 1 is the default icon but if we get more icon types this may change such as in-progress and ms_duration is how long to show it for if the user does not click it

