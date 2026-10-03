#undef UNICODE
#undef UNICODE
#define INITGUID

#include <vector>
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <iostream>
#include <regex>
#include <wininet.h>
#include <filesystem>
#include <commdlg.h>
#include <shlobj.h>
#include <ctime>
#include <algorithm>
#include <fstream>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")

#define _WIN32_DCOM
#define ID_DRAG_BUTTON 1
#define ID_LINK_BUTTON 2
#define ID_RESET_BUTTON 3
#define ID_CHECK_BUTTON 4
#define ID_ACCEPT_BUTTON 5
#define ID_CANCEL_BUTTON 6
#define ID_DOWNLOAD_BUTTON 7  
#define ID_COVER_BUTTON 8
#define ID_EXPLAIN_BUTTON 9



// Global variables
ULONG_PTR gdiToken;

HFONT hFont;
HWND hLinkBox;
HWND hDragButton;
HWND hLinkButton;
HWND hCaption;
HWND hResetButton;
HWND hCheckButton;
HWND hVideoTitle;
HWND hAcceptButton;
HWND hCancelButton;
HWND hLinkCaption;
HWND hDownloadButton;
HWND hTitleBox;
HWND hArtistBox;
HWND hAlbumBox;
HWND hCoverButton;
HWND hTitleCaption; 
HWND hArtistCaption;
HWND hAlbumCaption;
HWND hFileLabel;
HWND hCoverLabel;
HWND hCoverPreview;
HWND hExplainBox;

bool isValidURL(HWND hwnd);
bool isLocalFile = false;

void UpdateUI(HWND hwnd);
void reset(HWND hwnd);
void CheckVideoLink(HWND hwnd);
void ShowVideoPreview(HWND hwnd);
void PlayVideo(HWND hwnd, std::string url);
void DownloadMP3(
    HWND hwnd,
    std::string url,
    std::string title,
    std::string artist,
    std::string album
);

std::string coverPath;
std::string ChooseSaveLocation(HWND hwnd);
std::string ChooseAudioVideo(HWND hwnd);
std::string ChooseCover(HWND hwnd);
std::string GetVideoTitleFromYtDlp(const std::string& ytDlpPath, const std::string& url);
std::string filePath;
std::string ytDlpPath;

// Define an enumeration for the file state
enum fileState
{
    WAITING_FOR_FILE,
    FILE_SELECTED
};

// Define an enumeration for the application state
enum AppState
{
    STATE_MENU,
    STATE_DRAG,
    STATE_LINK,
    STATE_LINK_REVIEW,
    STATE_AUDIO_CREDENTIALS
};

AppState state = STATE_MENU;

// Define a structure to hold song information
struct SongInfo
{
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int year;
};

Gdiplus::Image* coverImage = nullptr;


// The window procedure to handle messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

        // Create buttons when window starts
        case WM_CREATE:
        {
            // Create a font for the caption
            hFont = CreateFont(
                24, 0, 0, 0,
                FW_BOLD,
                FALSE, FALSE, FALSE,
                ANSI_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_SWISS,
                "Arial"
            );
            
            hLinkBox = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_BORDER | ES_LEFT,
                50, 200,
                400, 30,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            // Create a button to drag the file
            hDragButton = CreateWindow(
                "BUTTON",
                "Drag File",
                WS_CHILD,
                100, 100,
                300, 50,
                hwnd,
                (HMENU)ID_DRAG_BUTTON,
                NULL,
                NULL
            );

            // Create a button to link the file
            hLinkButton = CreateWindow(
                "BUTTON",
                "Link File",
                WS_CHILD,
                100, 250,
                300, 50,
                hwnd,
                (HMENU)ID_LINK_BUTTON,
                NULL,
                NULL
            );
            // Create a caption
            hCaption = CreateWindow(
                "STATIC",
                "Convert Audio to Local files",
                WS_CHILD | SS_CENTER,
                100, 20,
                300, 50,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            hLinkCaption = CreateWindow(
                "STATIC",
                "Paste the link of the video you want to convert to MP3",
                WS_CHILD | SS_CENTER,
                100, 100,
                300, 50,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            // Create a reset button
            hResetButton = CreateWindow(
                "BUTTON",
                "Reset",
                WS_CHILD,
                0, 0,
                200, 50,
                hwnd,
                (HMENU)ID_RESET_BUTTON,
                NULL,
                NULL
            );

            hCheckButton = CreateWindow(
                "BUTTON",
                "Enter",
                WS_CHILD,
                100, 300,
                300, 30,
                hwnd,
                (HMENU)ID_CHECK_BUTTON,
                NULL,
                NULL
            );

            hVideoTitle = CreateWindow(
                "STATIC",
                "Enter the video URL:",
                WS_CHILD | SS_LEFT,
                100, 170,
                300, 40,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            hAcceptButton = CreateWindow(
                "BUTTON",
                "Accept",
                WS_CHILD,
                100, 350,
                100, 30,
                hwnd,
                (HMENU)ID_ACCEPT_BUTTON,
                NULL,
                NULL
            );

            hCancelButton = CreateWindow(
                "BUTTON",
                "Cancel",
                WS_CHILD,
                200, 350,
                100, 30,
                hwnd,
                (HMENU)ID_CANCEL_BUTTON,
                NULL,
                NULL
            );

            hDownloadButton = CreateWindow(
                "BUTTON",
                "Download MP3",
                WS_CHILD,
                100, 400,
                200, 30,
                hwnd,
                (HMENU)ID_DOWNLOAD_BUTTON,  // FIX 1 (usage): was (HMENU)ID DOWNLOAD_BUTTON
                NULL,
                NULL
            );

            hTitleCaption = CreateWindow(
                "STATIC",
                "Song Title",
                WS_CHILD | WS_BORDER,
                50, 115,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );
            
            hTitleBox = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_BORDER,
                50, 140,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );


            hArtistBox = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_BORDER,
                50, 220,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );
                       
            hArtistCaption = CreateWindow(
                "STATIC",
                "Artist(s)",
                WS_CHILD | WS_BORDER,
                50, 195,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            hAlbumBox = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_BORDER,
                50, 300,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            hAlbumCaption = CreateWindow(
                "STATIC",
                "Album",
                WS_CHILD | WS_BORDER,
                50, 275,
                350, 25,
                hwnd,
                NULL,
                NULL,
                NULL
            );
            hCoverButton = CreateWindow(
                "BUTTON",
                "Choose Cover",
                WS_CHILD,
                300, 400,
                150, 30,
                hwnd,
                (HMENU)ID_COVER_BUTTON,
                NULL,
                NULL
            );
            hFileLabel = CreateWindow(
                "STATIC",
                "",
                WS_CHILD | SS_LEFT,
                100, 60,
                450, 30,
                hwnd,
                NULL,
                NULL,
                NULL
            );

            hCoverLabel = CreateWindow(
                "STATIC",
                "",
                WS_CHILD | SS_LEFT,
                250, 450,
                450, 30,
                hwnd,
                NULL,
                NULL,
                NULL
                );
            hCoverPreview = CreateWindow(
                "STATIC",
                "",
                WS_CHILD | SS_BITMAP | SS_CENTERIMAGE,
                350, 120,
                200, 200,
                hwnd,
                NULL,
                NULL,
                NULL
                );
                        // Create a reset button
            hExplainBox = CreateWindow(
                "BUTTON",
                "How it works",
                WS_CHILD,
                450, -2,
                150, 50,
                hwnd,
                (HMENU)ID_EXPLAIN_BUTTON,
                NULL,
                NULL
            );
            

            // Set the font for the caption
            SendMessage(hCaption, WM_SETFONT, (WPARAM)hFont, TRUE);

            UpdateUI(hwnd);

            return 0;
        }

        // Handle button clicks
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case ID_DRAG_BUTTON:
                {
                    //Opens the file you want imported
                    filePath = ChooseAudioVideo(hwnd);

                    if (!filePath.empty())
                    {
                        std::string fileNameOnly =
                            std::filesystem::path(filePath).filename().string();
                        SetWindowText(hFileLabel, fileNameOnly.c_str());
                        UpdateUI(hwnd);
                    }
                    state = STATE_DRAG;
                    UpdateUI(hwnd);
                    break;
                }

                case ID_LINK_BUTTON:
                    state = STATE_LINK;
                    UpdateUI(hwnd);
                    break;
                
                case ID_CHECK_BUTTON:
                     if (isValidURL(hwnd)) {
                        isLocalFile = false;
                        state = STATE_LINK_REVIEW;
                        UpdateUI(hwnd);
                        ShowVideoPreview(hwnd);
                    } else {
                        MessageBox(hwnd, "Please enter a valid URL.", "Invalid URL", MB_OK);
                    }
                    break;

                case ID_ACCEPT_BUTTON:
                {
                    // Hide Accept / Cancel after accepting
                    ShowWindow(hAcceptButton, SW_HIDE);
                    ShowWindow(hCancelButton, SW_HIDE);
                    ShowWindow(hVideoTitle, SW_HIDE);

                    // Show tag editing fields
                    state = STATE_AUDIO_CREDENTIALS;
                    UpdateUI(hwnd);

                    break;
                }

                case ID_DOWNLOAD_BUTTON:
                {

                    char urlBuf[512];

                    GetWindowText(
                        hLinkBox,
                        urlBuf,
                        512
                    );

                    std::string url(urlBuf);


                    char titleBuf[256];
                    char artistBuf[256];
                    char albumBuf[256];

                    //Saves the Title artist and album names enetered to the Audio tag
                    GetWindowText(
                        hTitleBox,
                        titleBuf,
                        256
                    );

                    GetWindowText(
                        hArtistBox,
                        artistBuf,
                        256
                    );

                    GetWindowText(
                        hAlbumBox,
                        albumBuf,
                        256
                    );

                    // Stores the results
                    std::string titleStr(titleBuf);
                    std::string artistStr(artistBuf);
                    std::string albumStr(albumBuf);

                    //Downloads everything togehter
                    DownloadMP3(
                        hwnd,
                        url,
                        titleStr,
                        artistStr,
                        albumStr
                    );


                    break;
                }


                case ID_CANCEL_BUTTON:
                    state = STATE_LINK;
                    UpdateUI(hwnd);
                    break;

                case ID_RESET_BUTTON:
                    reset(hwnd);
                    break;
                
                case ID_COVER_BUTTON:
                {
                    coverPath = ChooseCover(hwnd);
                     if (!coverPath.empty())
                    {
                        std::string fileNameOnly =
                            std::filesystem::path(coverPath).filename().string();

                        SetWindowText(hCoverLabel, fileNameOnly.c_str());

                        // delete old image to avoid memory leak
                        if (coverImage)
                        {
                            delete coverImage;
                            coverImage = nullptr;
                        }

                        // Convert std::string to wide string
                        std::wstring wpath(coverPath.begin(), coverPath.end());

                        // Load image into memory
                        coverImage = new Gdiplus::Image(wpath.c_str());
                        }
                    UpdateUI(hwnd);
                    InvalidateRect(hwnd, nullptr, TRUE);
                    return 0;
                }
                case ID_EXPLAIN_BUTTON:
                 MessageBox(hwnd,
                    "Audio Tag Creator - How it works:\n\n"
                    "1. DRAG FILE: Import a local MP3, MP4, or audio file from your computer.\n\n"
                    "2. LINK FILE: Paste a YouTube or video URL to download it as an MP3.\n\n"
                    "3. Fill in the Song Title, Artist, and Album fields.\n\n"
                    "4. Optionally choose a cover image.\n\n"
                    "5. Click Download MP3 and choose where to save it.\n\n"
                    "Note: yt-dlp.exe and ffmpeg.exe must be in the same folder as this program.",
                    "How it works",
                    MB_OK
                );
                break;
            }
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (coverImage)
            {
                Gdiplus::Graphics g(hdc); // FIX: fully qualified

                RECT coverBox = {400, 120, 560, 280};

                int boxW = coverBox.right - coverBox.left;
                int boxH = coverBox.bottom - coverBox.top;

                UINT imgW = coverImage->GetWidth();
                UINT imgH = coverImage->GetHeight();

                float scale = (std::min)((float)boxW / imgW, (float)boxH / imgH);

                int drawW = (int)(imgW * scale);
                int drawH = (int)(imgH * scale);

                int x = coverBox.left + (boxW - drawW) / 2;
                int y = coverBox.top + (boxH - drawH) / 2;

                g.DrawImage(coverImage, x, y, drawW, drawH);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}


// Function to update the UI based on the current state
void UpdateUI(HWND hwnd)
{
    if (state == STATE_MENU)
    {
        ShowWindow(hCaption, SW_SHOW);
        ShowWindow(hDragButton, SW_SHOW);
        ShowWindow(hLinkButton, SW_SHOW);
        ShowWindow(hLinkBox, SW_HIDE);
        ShowWindow(hResetButton, SW_HIDE);
        ShowWindow(hCheckButton, SW_HIDE);
        ShowWindow(hLinkCaption, SW_HIDE);
        ShowWindow(hAlbumBox, SW_HIDE);
        ShowWindow(hTitleBox, SW_HIDE);
        ShowWindow(hArtistBox, SW_HIDE);
        ShowWindow(hDownloadButton, SW_HIDE);
        ShowWindow(hCoverButton, SW_HIDE);  
        ShowWindow(hAcceptButton, SW_HIDE);
        ShowWindow(hCancelButton, SW_HIDE);
        ShowWindow(hArtistCaption, SW_HIDE);  
        ShowWindow(hAlbumCaption, SW_HIDE);  
        ShowWindow(hTitleCaption, SW_HIDE);  
        ShowWindow(hFileLabel, SW_HIDE);
        ShowWindow(hCoverLabel, SW_HIDE);
        ShowWindow(hCoverPreview, SW_HIDE);
        ShowWindow(hExplainBox, SW_SHOW);
    }
    else if (state == STATE_LINK)
    {
        ShowWindow(hCaption, SW_HIDE);
        ShowWindow(hDragButton, SW_HIDE);
        ShowWindow(hLinkButton, SW_HIDE);
        ShowWindow(hLinkBox, SW_SHOW);
        ShowWindow(hResetButton, SW_SHOW);
        ShowWindow(hCheckButton, SW_SHOW);
        ShowWindow(hVideoTitle, SW_HIDE);
        ShowWindow(hAcceptButton, SW_HIDE);
        ShowWindow(hCancelButton, SW_HIDE);
        ShowWindow(hLinkCaption, SW_SHOW);
        ShowWindow(hVideoTitle, SW_HIDE);
        ShowWindow(hAcceptButton, SW_HIDE);
        ShowWindow(hCancelButton, SW_HIDE);
        ShowWindow(hFileLabel, SW_HIDE);
        ShowWindow(hCoverLabel, SW_HIDE);
        ShowWindow(hExplainBox, SW_SHOW);
    }
    else if (state == STATE_DRAG)
    {
        ShowWindow(hCaption, SW_HIDE);
        ShowWindow(hDragButton, SW_HIDE);
        ShowWindow(hLinkButton, SW_HIDE);
        ShowWindow(hLinkBox, SW_HIDE);
        ShowWindow(hResetButton, SW_SHOW);
        ShowWindow(hCheckButton, SW_HIDE);
        ShowWindow(hFileLabel, SW_SHOW);
        ShowWindow(hCoverLabel, SW_HIDE);
        ShowWindow(hExplainBox, SW_SHOW);
    }
    else if (state == STATE_AUDIO_CREDENTIALS)
    {
        ShowWindow(hCaption, SW_HIDE);
        ShowWindow(hDragButton, SW_HIDE);
        ShowWindow(hLinkButton, SW_HIDE);
        ShowWindow(hLinkBox, SW_HIDE);
        ShowWindow(hResetButton, SW_SHOW);
        ShowWindow(hCheckButton, SW_HIDE);
        ShowWindow(hTitleBox, SW_SHOW);
        ShowWindow(hTitleCaption, SW_SHOW);
        ShowWindow(hArtistBox, SW_SHOW);  
        ShowWindow(hArtistCaption, SW_SHOW);  
        ShowWindow(hAlbumBox, SW_SHOW);
        ShowWindow(hAlbumCaption, SW_SHOW);  
        ShowWindow(hDownloadButton, SW_SHOW);
        ShowWindow(hCoverButton, SW_SHOW);  
        ShowWindow(hFileLabel, SW_SHOW);
        ShowWindow(hCoverLabel, SW_SHOW);
        ShowWindow(hExplainBox, SW_SHOW);
    }
    else if (state == STATE_LINK_REVIEW)
    {
        ShowWindow(hLinkBox, SW_HIDE);
        ShowWindow(hCheckButton, SW_HIDE);
        ShowWindow(hLinkCaption, SW_HIDE);

        // Show video confirmation buttons
        ShowWindow(hVideoTitle, SW_SHOW);
        ShowWindow(hAcceptButton, SW_SHOW);
        ShowWindow(hCancelButton, SW_SHOW);

        // Keep reset available
        ShowWindow(hResetButton, SW_SHOW);

        // Hide audio metadata boxes until accepted
        ShowWindow(hTitleBox, SW_HIDE);
        ShowWindow(hArtistBox, SW_HIDE);
        ShowWindow(hAlbumBox, SW_HIDE);

        ShowWindow(hExplainBox, SW_SHOW);
    }

    RedrawWindow(hwnd, NULL, NULL,
        RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    UpdateWindow(hwnd);
}

bool isValidURL(HWND hwnd)
{
    char buffer[512];

    GetWindowText(
        hLinkBox,
        buffer,
        512
    );

    std::string link = buffer;


    // Check if the link is empty
    if(link.empty())
    {
        MessageBox(hwnd, "Please enter a URL.", "Invalid URL", MB_OK);
        return false;
    }


    // Accept normal video websites and direct video files
    std::regex url_regex(
        R"(^(https?://).*(youtube|youtu\.be|vimeo|dailymotion|twitch|streamable|facebook|instagram|\.mp4|\.webm|\.mov|\.avi|\.mkv|\.flv|\.wmv).*)",
        std::regex::icase
    );


    if(std::regex_match(link, url_regex))
    {
        return true;
    }


    return false;
}

//Checks to see if the video link can open a the video 
void CheckVideoLink(HWND hwnd)
{
    if(isValidURL(hwnd)) {
         return;
    }
    MessageBox(hwnd, "Please enter a valid URL.", "Invalid URL. Try again.", MB_OK);
}

// Placeholder function to extract video title from URL
std::string getVideoTitle(const std::string& url)
{
    // Find yt-dlp.exe next to the program
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string folder(exePath);
    folder = folder.substr(0, folder.find_last_of("\\/"));

    std::string ytDlp = folder + "\\yt-dlp.exe";

    if (!std::filesystem::exists(ytDlp))
    {
        return "Unknown Title";
    }

    // Pipe so we can read what yt-dlp prints
    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE hRead = NULL;
    HANDLE hWrite = NULL;

    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        return "Unknown Title";
    }

    // The read end should not be inherited by yt-dlp
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    // Errors go to NUL so they don't end up in the title
    HANDLE hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_WRITE, &sa, OPEN_EXISTING, 0, NULL);

    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hWrite;
    si.hStdError = hNull;

    std::string command =
        "\"" + ytDlp + "\" --get-title --no-playlist \"" + url + "\"";

    std::vector<char> cmd(command.begin(), command.end());
    cmd.push_back('\0');

    BOOL success = CreateProcessA(NULL, cmd.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    // Close our copy of the write end so ReadFile ends when yt-dlp exits
    CloseHandle(hWrite);
    if (hNull != INVALID_HANDLE_VALUE) CloseHandle(hNull);

    if (!success)
    {
        CloseHandle(hRead);
        return "Unknown Title";
    }

    char buffer[1024];
    DWORD bytesRead;

    std::string content;

    while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead)
    {
        content.append(buffer, bytesRead);
    }

    // Now close after reading
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hRead);

    // Trim trailing newline characters
    while (!content.empty() && (content.back() == '\n' || content.back() == '\r'))
    {
        content.pop_back();
    }

    if (exitCode != 0 || content.empty())
    {
        return "Unknown Title";
    }

    return content;
}

// Function to show video preview and title
void ShowVideoPreview(HWND hwnd)
{

    int len = GetWindowTextLength(hLinkBox) + 1;
    std::string link(len, '\0');
    GetWindowText(hLinkBox, &link[0], len);

    std::string videoTitle = getVideoTitle(link);

    SetWindowText(hVideoTitle, ("Video Title: " + videoTitle).c_str());
    SetWindowText(hFileLabel, videoTitle.c_str());  

    state = STATE_LINK_REVIEW;
    UpdateUI(hwnd);

}

// Function to play the video using Windows Media Player
void PlayVideo(HWND hwnd, std::string url)
{
    ShellExecute(hwnd, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// Resets the application to the initial state
void reset(HWND hwnd)
{
    state = STATE_MENU;
    // Clear cover image
    if (coverImage)
    {
        delete coverImage;
        coverImage = nullptr;
    }
    coverPath = "";

    // Clear labels
    SetWindowText(hFileLabel, "");
    SetWindowText(hCoverLabel, "");
    SetWindowText(hTitleBox, "");
    SetWindowText(hArtistBox, "");
    SetWindowText(hAlbumBox, "");

    InvalidateRect(hwnd, nullptr, TRUE);
    UpdateUI(hwnd);
    isLocalFile = false;
}

// Converts video URL to mp3 using bundled ffmpeg.exe
void DownloadMP3(HWND hwnd, std::string url, std::string title, std::string artist, std::string album)
{
    std::string output = ChooseSaveLocation(hwnd);
    if (output.empty()) return;

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string folder(exePath);
    folder = folder.substr(0, folder.find_last_of("\\/"));

    std::string ffmpegPath = folder + "\\ffmpeg.exe";

    if (!std::filesystem::exists(ffmpegPath))
    {
        MessageBox(hwnd, "ffmpeg.exe was not found next to the program", "Missing FFmpeg", MB_OK);
        return;
    }

    std::string command;

    if (isLocalFile)
    {
        // Local file — use ffmpeg directly to re-encode and tag
        std::string logFile = folder + "\\ffmpeg_log.txt";

        command =
            "cmd.exe /C \""
            "\"" + ffmpegPath + "\" "
            "-i \"" + filePath + "\" "
            "-vn "
            "-map 0:a:0 "
            "-c:a libmp3lame "
            "-q:a 2 "
            "-metadata title=\"" + title + "\" "
            "-metadata artist=\"" + artist + "\" "
            "-metadata album=\"" + album + "\" "
            "-y "
            "\"" + output + "\""
            " > \"" + logFile + "\" 2>&1\"";
    }
    else
    {
        // URL — use yt-dlp
        std::string ytDlpPath = folder + "\\yt-dlp.exe";

        if (!std::filesystem::exists(ytDlpPath))
        {
            MessageBox(hwnd, "yt-dlp.exe was not found next to the program", "Missing yt-dlp", MB_OK);
            return;
        }

        std::string outputTemplate =
            std::filesystem::path(output).replace_extension("").string() + ".%(ext)s";

        command =
            "\"" + ytDlpPath + "\" "
            "--no-playlist "
            "-x "
            "--audio-format mp3 "
            "--ffmpeg-location \"" + folder + "\" "
            "--postprocessor-args \"ExtractAudio:-metadata title=\\\"" + title + "\\\" "
            "-metadata artist=\\\"" + artist + "\\\" "
            "-metadata album=\\\"" + album + "\\\"\" "
            "-o \"" + outputTemplate + "\" "
            "\"" + url + "\"";
    }

    // Send the process output to ytdlp_log.txt so errors can be read
    std::string ytLogFile = folder + "\\ytdlp_log.txt";

    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE hLog = CreateFileA(
        ytLogFile.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

        STARTUPINFOA si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hLog;
        si.hStdError = hLog;

        std::vector<char> cmd(command.begin(), command.end());
        cmd.push_back('\0');

        BOOL success = CreateProcessA(NULL, cmd.data(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

        if (!success)
        {
            MessageBox(hwnd, ("Could not start process\nError: " + std::to_string(GetLastError())).c_str(), "Error", MB_OK);
            return;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(hLog);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

    if (exitCode == 0)
    {
        // Embed cover if selected
        if (!coverPath.empty())
        {
            std::string tempOutput = output + ".temp.mp3";

            std::string ffmpegCmd =
                "\"" + ffmpegPath + "\" -i \"" + output + "\" "
                "-i \"" + coverPath + "\" "
                "-map 0:a -map 1:v "
                "-c copy "
                "-id3v2_version 3 "
                "-metadata:s:v title=\"Album cover\" "
                "-metadata:s:v comment=\"Cover (front)\" "
                "\"" + tempOutput + "\"";

            STARTUPINFOA si2 = {};
            PROCESS_INFORMATION pi2 = {};
            si2.cb = sizeof(si2);

            std::vector<char> cmd2(ffmpegCmd.begin(), ffmpegCmd.end());
            cmd2.push_back('\0');

            BOOL success2 = CreateProcessA(NULL, cmd2.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2);

            if (success2)
            {
                WaitForSingleObject(pi2.hProcess, INFINITE);
                CloseHandle(pi2.hProcess);
                CloseHandle(pi2.hThread);

                std::filesystem::remove(output);
                std::filesystem::rename(tempOutput, output);
            }
        }

        MessageBox(hwnd, ("MP3 created at:\n\n" + output).c_str(), "Complete", MB_OK);
    }
    else
    {
        // Read the log so the real yt-dlp error shows in the message box
        std::ifstream logIn(ytLogFile);
        std::string logText((std::istreambuf_iterator<char>(logIn)),
                             std::istreambuf_iterator<char>());

        // Keep only the last part so the box stays readable
        if (logText.size() > 800) logText = logText.substr(logText.size() - 800);

        MessageBox(hwnd, ("Process failed\nExit code: " + std::to_string(exitCode) + "\n\nyt-dlp said:\n" + logText).c_str(), "Error", MB_OK);
    }
}

// Choose where I want to save the file
std::string ChooseSaveLocation(HWND hwnd){
    char filename[MAX_PATH] = "";

    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "MP3 Files\0*.mp3\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "mp3";

    if(GetSaveFileName(&ofn)){
        return std::string(filename);
    }

    return "";

}

//Choose MP3 or video to import for drag option
std::string ChooseAudioVideo(HWND hwnd){
    char filename[MAX_PATH] = "";

    //Opens the file options in file explorer
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter =
        "Media Files\0"
        "*.mp3;*.mp4;*.wav;*.m4a;*.avi;*.mkv\0";


    ofn.Flags = OFN_FILEMUSTEXIST;


    if(GetOpenFileName(&ofn))
    {
        isLocalFile = true;
        state = STATE_AUDIO_CREDENTIALS;
        return std::string(filename);
    }

    

    return "";

}


// Choose album cover image
std::string ChooseCover(HWND hwnd)
{
    char filename[MAX_PATH] = "";

     //Opens the file options in file explorer
    OPENFILENAME ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;


    ofn.lpstrFilter =
        "Image Files\0"
        "*.jpg;*.png;*.jpeg;*.bmp\0";


    ofn.Flags = OFN_FILEMUSTEXIST;


    if(GetOpenFileName(&ofn))
    {
        return std::string(filename);
    }


    return "";
}

// Gets the video title using yt-dlp instead of HTML scraping
std::string GetVideoTitleFromYtDlp(const std::string& ytDlpPath, const std::string& url)
{
    if (!std::filesystem::exists(ytDlpPath))
        return "yt-dlp not found at: " + ytDlpPath;

    std::string logPath = "C:\\Users\\lawba\\Desktop\\ytdlp_log.txt";

    std::string command =
        "\"" + ytDlpPath + "\" --get-title \"" + url + "\" > \"" + logPath + "\" 2>&1";

    system(command.c_str());

    // Read the log file
    std::ifstream f(logPath);
    if (!f.is_open())
        return "could not open log file";

    std::string result((std::istreambuf_iterator<char>(f)),
                        std::istreambuf_iterator<char>());

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result.empty() ? "empty output" : result;
}

// The entry point of the application
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiToken, &gdiplusStartupInput, nullptr);

    CoInitialize(NULL);

    // Register the window class
    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "AudioTagWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        "AudioTagWindow",
        "Audio Tag Creator",
        WS_OVERLAPPEDWINDOW,

        100, 100,
        600, 520,

        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    // Message loop
    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    Gdiplus::GdiplusShutdown(gdiToken);
    return 0;
}


// cd "C:\Users\lawba\OneDrive\Desktop\Everything\Audio Tag"
// g++ localFile.cpp -o localFile -std=c++17 -mwindows -lgdiplus -lwininet -lcomdlg32 -lole32 -lshell32 -lgdi32
//.\localFile.exe
// .\localFile.exe