#include <windows.h>
#include <shobjidl.h>

/* 
a note on COM objects:
COM objects are C++ classes/structs
This means that it has methods
This is simulated in C via lpVtbl (vtable) which is an array of function pointers
Use the lpVtbl member of a struct to call methods via STRUCTNAME -> lpVtbl -> METHODNAME(args)
https://www.codeproject.com/Articles/13601/COM-in-plain-C

Under the hood, the pointer to the struct is also a pointer to an array of function pointers (lbVtbl)
The struct is therefore the size of the number of elements of the lbVtbl array * 8 plus the data in the struct which succeeds it

Whenever we call one of these methods, we have to pass in the object (which implicitly happens in OOP languages)
Actually, we pass in a pointer to the object, obviously i'm quite familiar with this

One more nuance is that whenever we pass a COM object in a function as an argument, it must always be &object
This implicitly happens in OOP languages (references in C++), but in C it must be explicitly stated that it's a reference

So whenever you take C++ COM object sample code, just follow this process:
change all the methods to -> lpVtbl -> methods
Add &obj as the first argument of every method
Change obj to &obj for all objects passed as arguments to functions or methods

That's it! (probably)

IFileDialog: https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifiledialog
*/

#include <stdio.h>
#include <windows.h>
#include <shobjidl.h>

typedef struct {
    char filename[263]; // maximum filepath is 260 characters (?)
    char openOrSave; // 0 - open, 1 - save
    int numExtensions; // number of extensions
    char **extensions; // array of allowed extensions (7 characters long max)
} win32FileDialogObject; // almost as bad of naming as the windows API

win32FileDialogObject win32FileDialog;

void win32FileDialogInit() {
    strcpy(win32FileDialog.filename, "null");
    win32FileDialog.openOrSave = 0; // open by default
    win32FileDialog.numExtensions = 0; // 0 means all extensions
    win32FileDialog.extensions = calloc(1, 8); // one extension
}

void win32FileDialogAddExtension(char *extension) {
    win32FileDialog.numExtensions += 1;
    win32FileDialog.extensions = realloc(win32FileDialog.extensions, win32FileDialog.numExtensions * 8);
    win32FileDialog.extensions[win32FileDialog.numExtensions - 1] = strdup(extension);
    // printf("%s\n", fileDialog.extensions[fileDialog.numExtensions - 1]); // strdup sanity check
}

int win32FileDialogPrompt(char openOrSave, char *filename) { // 0 - open, 1 - save
    win32FileDialog.openOrSave = openOrSave;
    HRESULT hr = CoInitializeEx(NULL, 0); // https://learn.microsoft.com/en-us/windows/win32/api/objbase/ne-objbase-coinit
    if (SUCCEEDED(hr)) {
        IFileDialog *fileDialog;
        IShellItem *psiResult;
        PWSTR pszFilePath = NULL;
        hr = CoCreateInstance(&CLSID_FileOpenDialog, NULL, CLSCTX_ALL, &IID_IFileOpenDialog, (void**) &fileDialog);
        if (SUCCEEDED(hr)) {
            unsigned short Open[5] = {79, 112, 101, 110, 0};
            unsigned short Save[5] = {83, 97, 118, 101, 0};
            fileDialog -> lpVtbl -> SetOptions(fileDialog, 0); // https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions
            if (openOrSave == 1 && strcmp(filename, "null") != 0) {
                // printf("triggered\n");
                int i = 0;
                unsigned short prename[260];
                while (filename[i] != '\0' && i < 260) {
                    prename[i] = filename[i];
                    i++;
                }
                prename[i] = '\0';
                fileDialog -> lpVtbl -> SetFileName(fileDialog, prename);
            }
            if (openOrSave == 0) {
                fileDialog -> lpVtbl -> SetOkButtonLabel(fileDialog, (LPCWSTR) Open);
                fileDialog -> lpVtbl -> SetTitle(fileDialog, (LPCWSTR) Open);
            } else {
                fileDialog -> lpVtbl -> SetOkButtonLabel(fileDialog, (LPCWSTR) Save);
                fileDialog -> lpVtbl -> SetTitle(fileDialog, (LPCWSTR) Save);
            }
            fileDialog -> lpVtbl -> Show(fileDialog, NULL);
            hr = fileDialog -> lpVtbl -> GetResult(fileDialog, &psiResult);
            if (SUCCEEDED(hr)){
                hr = psiResult -> lpVtbl -> GetDisplayName(psiResult, SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr)) {
                    int i = 0;
                    while (pszFilePath[i] != '\0' && i < 260) {
                        win32FileDialog.filename[i] = pszFilePath[i]; // convert from WCHAR to char
                        i++;
                    }
                    win32FileDialog.filename[i] = '\0';
                    CoTaskMemFree(pszFilePath);
                    return 0;
                }
                psiResult -> lpVtbl -> Release(psiResult);
            }
            fileDialog -> lpVtbl -> Release(fileDialog);
        } else {
            printf("ERROR - HRESULT: %lx\n", hr);
        }
        CoUninitialize();
    }
    return -1;
}