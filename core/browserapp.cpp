#include "browserapp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

BrowserApp::BrowserApp(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Navigation Bar
    QHBoxLayout *navLayout = new QHBoxLayout();
    
    backBtn = new QPushButton("⬅️", this);
    forwardBtn = new QPushButton("➡️", this);
    refreshBtn = new QPushButton("🔄", this);
    homeBtn = new QPushButton("🏠", this);
    
    urlBar = new QLineEdit(this);
    urlBar->setPlaceholderText("Enter URL (e.g., google.com, kernel.status)...");
    
    backBtn->setFixedSize(30, 30);
    forwardBtn->setFixedSize(30, 30);
    refreshBtn->setFixedSize(30, 30);
    homeBtn->setFixedSize(30, 30);
    
    navLayout->addWidget(backBtn);
    navLayout->addWidget(forwardBtn);
    navLayout->addWidget(refreshBtn);
    navLayout->addWidget(homeBtn);
    navLayout->addWidget(urlBar);
    
    // Web Display area using safe HTML text rendering
    webView = new QTextBrowser(this);
    webView->setStyleSheet("background-color: #ffffff; color: #1e1e2e; font-family: Arial;");

    mainLayout->addLayout(navLayout);
    mainLayout->addWidget(webView);

    // Styling
    QString btnStyle = "QPushButton { background-color: #313244; color: white; border-radius: 5px; font-size: 14px; border: none; } QPushButton:hover { background-color: #45475a; }";
    backBtn->setStyleSheet(btnStyle);
    forwardBtn->setStyleSheet(btnStyle);
    refreshBtn->setStyleSheet(btnStyle);
    homeBtn->setStyleSheet(btnStyle);
    urlBar->setStyleSheet("background-color: #313244; color: white; border: 1px solid #45475a; border-radius: 5px; padding: 4px; font-size: 13px;");

    // Connect control pipelines
    connect(urlBar, &QLineEdit::returnPressed, this, &BrowserApp::navigateToUrl);
    connect(homeBtn, &QPushButton::clicked, this, &BrowserApp::goHome);
    connect(refreshBtn, &QPushButton::clicked, this, &BrowserApp::navigateToUrl);
    connect(backBtn, &QPushButton::clicked, webView, &QTextBrowser::backward);
    connect(forwardBtn, &QPushButton::clicked, webView, &QTextBrowser::forward);

    goHome(); // Load default home page on startup
}

void BrowserApp::navigateToUrl() {
    QString url = urlBar->text().trimmed().toLower();
    if(url.isEmpty()) return;
    loadMockPage(url);
}

void BrowserApp::goHome() {
    urlBar->setText("educloud.home");
    loadMockPage("educloud.home");
}
void BrowserApp::loadMockPage(const QString &url) {
    QString html;
    QString cleanedUrl = url.trimmed().toLower();

    // 1. HOME PORTAL
    if (cleanedUrl == "educloud.home" || cleanedUrl.isEmpty() || cleanedUrl == "home") {
        html = R"(
            <html><body style='padding: 20px; background-color: #f4f4f9; font-family: Arial, sans-serif; color: #333;'>
                <div style='background: #89b4fa; color: #11111b; padding: 20px; border-radius: 8px; margin-bottom: 20px;'>
                    <h1 style='margin: 0;'>🌐 EduCloud Web Gateway</h1>
                    <p style='margin: 5px 0 0 0;'>Virtual Network Routing Engine Enabled</p>
                </div>
                <h3>🔥 Popular Web Nodes:</h3>
                <table style='width: 100%; border-collapse: collapse;'>
                    <tr style='background: #e4e4ed;'><td style='padding: 10px;'><b>Address</b></td><td style='padding: 10px;'><b>Description</b></td></tr>
                    <tr><td style='padding: 10px;'><a href='google.com' style='color:#89b4fa; font-weight:bold;'>google.com</a></td><td style='padding: 10px;'>Simulated Search Engine Workspace</td></tr>
                    <tr><td style='padding: 10px;'><a href='wikipedia.org' style='color:#89b4fa; font-weight:bold;'>wikipedia.org</a></td><td style='padding: 10px;'>The Free Operating System Encyclopedia</td></tr>
                    <tr><td style='padding: 10px;'><a href='ai.cloud' style='color:#89b4fa; font-weight:bold;'>ai.cloud</a></td><td style='padding: 10px;'>EduCloud Virtual AI Chat Companion</td></tr>
                    <tr><td style='padding: 10px;'><a href='kernel.status' style='color:#89b4fa; font-weight:bold;'>kernel.status</a></td><td style='padding: 10px;'>OS Kernel Core Diagnostics Panel</td></tr>
                </table>
            </body></html>
        )";
    } 
    // 2. GOOGLE MAIN PAGE
    else if (cleanedUrl == "google.com" || cleanedUrl == "www.google.com") {
        html = R"(
            <html><body style='text-align: center; padding-top: 50px; background-color: #ffffff; font-family: sans-serif;'>
                <h1 style='font-size: 55px; margin-bottom: 15px;'>
                    <span style='color: #4285F4;'>G</span><span style='color:#EA4335;'>o</span>
                    <span style='color:#FBBC05;'>o</span><span style='color:#4285F4;'>g</span>
                    <span style='color:#34A853;'>l</span><span style='color:#EA4335;'>e</span>
                </h1>
                <p style='color: #777; font-size: 14px;'>Type any search query in the address bar above and press Enter!</p>
                <div style='margin-top: 20px;'>
                    <span style='background:#f8f9fa; padding: 10px 20px; border-radius:4px; font-size:13px; color:#3c4043; border: 1px solid #f8f9fa; margin: 5px;'>Google Search</span>
                    <span style='background:#f8f9fa; padding: 10px 20px; border-radius:4px; font-size:13px; color:#3c4043; border: 1px solid #f8f9fa; margin: 5px;'>I'm Feeling Lucky</span>
                </div>
            </body></html>
        )";
    } 
    // 3. WIKIPEDIA
    else if (cleanedUrl.contains("wikipedia")) {
        html = R"(
            <html><body style='padding: 25px; background-color: #ffffff; font-family: serif; color: #000;'>
                <h1 style='border-bottom: 1px solid #a2a9b1; font-family: sans-serif;'>📋 Wikipedia: Operating Systems</h1>
                <p>An <b>Operating System (OS)</b> is system software that manages computer hardware, software resources, and provides common services for computer programs.</p>
                <h3>Core Concepts:</h3>
                <ul>
                    <li><b>Kernel:</b> The core controller of the architecture, managing process scheduling and memory matrices.</li>
                    <li><b>File System:</b> Structuring mechanical or solid-state memory indexes (like your custom File Explorer!).</li>
                    <li><b>IPC:</b> Inter-Process Communication allowing threads to sync execution flags safely.</li>
                </ul>
                <p style='font-size: 12px; color: #555; margin-top: 30px;'>Return to <a href='educloud.home'>Home Gateway</a></p>
            </body></html>
        )";
    }
    // 4. MOCK AI COMPANION
    else if (cleanedUrl.contains("ai.cloud")) {
        html = R"(
            <html><body style='padding: 20px; background-color: #1a1b26; color: #a9b1d6; font-family: Arial;'>
                <div style='border: 1px solid #7aa2f7; padding: 15px; border-radius: 8px; background: #24283b;'>
                    <h2 style='color: #7aa2f7; margin-top:0;'>🤖 EduCloud AI Assistant v1.0</h2>
                    <p style='font-style: italic; color: #565f89;'>Status: Online & Connected to Local OS Event Loops</p>
                    <hr style='border: 0; border-top: 1px solid #565f89;'>
                    <p><b>User:</b> Explain my operating system project.</p>
                    <p style='color: #9ece6a;'><b>AI:</b> Your project is a distributed-style Operating System simulation built using C++ and Qt. It successfully showcases multi-task scheduling algorithms, a virtual sandboxed file system, active thread process monitoring, and an elegant responsive graphical user shell environment!</p>
                </div>
                <p style='text-align: center; font-size:12px;'><a href='educloud.home' style='color:#7aa2f7;'>Back to Home Node</a></p>
            </body></html>
        )";
    }
    // 5. KERNEL DIAGNOSTICS
    else if (cleanedUrl == "kernel.status") {
        html = R"(
            <html><body style='padding: 20px; background-color: #11111b; color: #a6e3a1; font-family: monospace;'>
                <h2>[EDUCLOUD HARDWARE SYSTEM BUS MATRIX]</h2>
                <p>STATUS: ACTIVE (0x00FF12)</p>
                <p>----------------------------------------</p>
                <p>-> MDI Workspace Threads : ALIVE (Priority High)</p>
                <p>-> Virtual File Map Allocation : 100% Secure integrity check passed</p>
                <p>-> UI Render Engine Pipelines : Anchored to main event loop context</p>
                <p>----------------------------------------</p>
                <p>👉 <a href='educloud.home' style='color: #89b4fa;'>Exit Diagnostics</a></p>
            </body></html>
        )";
    }
    // 6. DYNAMIC GOOGLE SEARCH RESULTS GENERATOR
    else {
        // If they search for something random, treat it as a Google Search Result page!
        QString query = urlBar->text();
        html = QString(R"(
            <html><body style='padding: 20px; background-color: #ffffff; font-family: sans-serif; color: #222;'>
                <h2 style='color: #1a0dab; font-size: 20px;'>🔍 Google Search Results for: "%1"</h2>
                <p style='color: #006621; font-size: 13px; margin: 0;'>About 4,200 virtual results found inside memory cache</p>
                
                <div style='margin-top: 20px;'>
                    <a href='wikipedia.org' style='color: #1a0dab; font-size: 18px; text-decoration: none;'><b>Best Configurations for %1</b></a>
                    <p style='color: #545454; font-size: 13px; margin: 4px 0;'>Explore top framework patterns, documentation layouts, and architecture components dealing directly with %1 processing protocols.</p>
                </div>

                <div style='margin-top: 20px;'>
                    <a href='ai.cloud' style='color: #1a0dab; font-size: 18px; text-decoration: none;'><b>Ask the AI Expert about %1</b></a>
                    <p style='color: #545454; font-size: 13px; margin: 4px 0;'>Get an instant deep-dive breakdown regarding engineering, algorithms, and practical debugging methods for %1.</p>
                </div>

                <div style='margin-top: 40px; border-top: 1px solid #eee; padding-top: 10px;'>
                    <p style='font-size: 13px;'>Not what you were looking for? Return <a href='educloud.home' style='color: #1a0dab;'>Home</a> or search another node.</p>
                </div>
            </body></html>
        )").arg(query);
    }

    webView->setHtml(html);
}