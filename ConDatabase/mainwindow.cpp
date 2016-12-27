#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "completelineedit.h"
#include <QPushButton>
#include <QMessageBox>
#include <QMargins>
#include <QLineEdit>
#include<QDragEnterEvent>
#include<QUrl>
#include<QTextStream>
#include<QString>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    initlayout();//初始化布局
    createMenu();
    connect(cbo_sex, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_sel_sex(const QString &)));
    connect(edit, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(pSearchButton,SIGNAL(clicked(bool)),this,SLOT(search()));
    connect(fileTreeview,SIGNAL(getText(QString)),this,SLOT(setText(QString)));

    QTimer *timer = new QTimer(this);//新建定时器
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));//关联定时器计满信号和相应的槽函数
    timer->start(1000);//定时器开始计时，其中1000表示1000ms即1秒


   mediaplayer =new QMediaPlayer();

}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenu()
{
    creatFileMenu();
    creatEditMenu();
    creatAboutMenu();
}

//整合1
void MainWindow::creatFileMenu()
{

    newFileAction = new QAction(QIcon(":/myImages/images/document-icon.png"),tr("新建"),this);
    newFileAction->setShortcut(QKeySequence::New);
    openFileAction = new QAction(QIcon(":/myImages/images/folder-icon.png"),tr("打开"),this);
    openFileAction->setShortcut(QKeySequence::Open);
    closeAction = new QAction(QIcon(":/myImages/images/power-icon.png"),tr("关闭"),this);
    closeAction->setShortcut(QKeySequence::Close);
    saveAction = new QAction(QIcon(":/myImages/images/memorycard-icon.png"),tr("保存"),this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction = new QAction(QIcon(":/myImages/images/frames-icon.png"),tr("另存为"),this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    printAction=new QAction(QIcon(":/myImages/images/Polaroid-icon.png"),tr("打印"),this);

    quitAction=new QAction(QIcon(":/myImages/images/running-icon.png"),tr("退出"),this);
    quitAction->setShortcut(QKeySequence::Quit);

    mFileMenu = this->menuBar()->addMenu(tr("文件(F)"));
    mFileMenu->addAction(newFileAction);
    mFileMenu->addAction(openFileAction);
    mFileMenu->addAction(closeAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(saveAction);
    mFileMenu->addAction(saveAsAction);
    mFileMenu->addAction(printAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(quitAction);

}

void MainWindow::creatEditMenu()
{
    undoAction = new QAction(QIcon(":/myImages/images/undo.png"),tr("撤销"),this);
    undoAction->setShortcut(QKeySequence::Undo);
    cutAction = new QAction(QIcon(":/myImages/images/cut.png"),tr("剪切"),this);
    cutAction->setShortcut(QKeySequence::Cut);
    copyAction = new QAction(QIcon(":/myImages/images/copy.png"),tr("复制"),this);
    copyAction->setShortcut(QKeySequence::Copy);
    pastAction=new QAction(QIcon(":/myImages/images/past.png"),tr("粘贴"),this);
    pastAction->setShortcut(QKeySequence::Paste);
    findAction=new QAction(QIcon(":/myImages/images/magnifying-glass-icon.png"),tr("查找"),this);

    mEditMenu = this->menuBar()->addMenu(tr("编辑(&E)"));
    mEditMenu->addAction(undoAction);
    mEditMenu->addAction(cutAction);
    mEditMenu->addSeparator();
    mEditMenu->addAction(copyAction);
    mEditMenu->addAction(pastAction);
    mEditMenu->addSeparator();
    mEditMenu->addAction(findAction);
}

void MainWindow::creatAboutMenu()
{
    aboutActin=new QAction(QIcon(":/myImages/images/about.png"),tr("关于"),this);
    mAboutMenu = this->menuBar()->addMenu(tr("帮助(&H)"));
    mAboutMenu->addAction(aboutActin);

}

SetToolBtn::SetToolBtn(QString background, QString toolTips,QWidget *parent):
    QToolButton(parent)
{
    //    normalPixmap.load(background);
    this->setIcon(QIcon(background));
    this->setIconSize(QSize(30,30));
    this->setFixedSize(30,30);
    this->setAutoRaise(true);
    this->setToolTip(toolTips);
    this->setContentsMargins(0,0,0,0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


QFileInfoList MainWindow::getFileList(const QString&path)
{
    QDir dir(path);
    QString filter;
    QFileInfoList file_list =dir.entryInfoList(QDir::Files|QDir::NoSymLinks);
    QFileInfoList folder_list =dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
    for(int i=0;i!=folder_list.size();i++)
    {
        QString name =folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list =getFileList(name);
        file_list.append(child_file_list);
    }
    foreach(QFileInfo fileinfo,file_list)
    {
        filter= fileinfo.suffix();
        if((filter!="xls")&&(filter!="txt")&&(filter!="jpg")&&(filter!="png"))
        {
            continue;
        }
        mStringlist.append(fileinfo.filePath());
    }
    return file_list;
}


void MainWindow::initlayout()
{


    cbo_sex = new QComboBox();
    cbo_sex->addItem(QWidget::tr("本地文件"));
    cbo_sex->addItem(QWidget::tr("数据库文件"));

    edit=new QLineEdit;
    completer =new QCompleter;
    model =new QDirModel;
    completer->setModel(model);
    edit->setCompleter(completer);
    pSearchButton=  new QPushButton();
    pSearchButton->setCursor(Qt::PointingHandCursor);
    pSearchButton->setFixedSize(22, 22);
    pSearchButton->setToolTip(tr("search"));
    pSearchButton->setStyleSheet("QPushButton{border-image:url(:/myImages/icon_search_normal.png); background:transparent;} \
                                 QPushButton:hover{border-image:url(:/myImages/icon_search_hover.png)} \
                                 QPushButton:pressed{border-image:url(:/myImages/icon_search_press.png)}");

    QMargins margins =edit->textMargins();
    edit->setTextMargins(margins.left(),margins.top(),pSearchButton->width(),margins.bottom());
    edit->setPlaceholderText("请输入搜索内容");
    QHBoxLayout *pSearchLayout2= new QHBoxLayout();
    pSearchLayout2->addStretch();
    pSearchLayout2->addWidget(pSearchButton);
    pSearchLayout2->setSpacing(0);
    pSearchLayout2->setContentsMargins(0, 0, 0, 0);
    edit->setLayout(pSearchLayout2);
    QHBoxLayout *pSearchLayout = new QHBoxLayout();
    pSearchLayout->setSpacing(10);
    pSearchLayout->addWidget(cbo_sex);
    pSearchLayout->addWidget(edit);

    mUpLeftDock = new QDockWidget(tr("我的电脑"),this);
    mUpRightDock = new QDockWidget(tr("文本编辑器"),this);
    mDownLeftDock = new QDockWidget(tr("数据库"),this);
    mDownRightDock = new QDockWidget(tr("浏览器"),this);


    setCentralWidget(mUpLeftDock);
    addDockWidget(Qt::LeftDockWidgetArea,mUpLeftDock);
    splitDockWidget(mUpLeftDock,mUpRightDock,Qt::Horizontal);
    splitDockWidget(mUpLeftDock,mDownLeftDock,Qt::Vertical);

    tabifyDockWidget(mUpRightDock,mDownRightDock);
    mUpLeftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    mDownLeftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    mUpRightDock->raise();
    mUpLeftDock->setMinimumWidth(250);
    mDownLeftDock->setMinimumWidth(250);
    mUpLeftDock->setMaximumWidth(250);
    mDownLeftDock->setMaximumWidth(250);



    fileTreeview =new FileTreeview();
    QVBoxLayout *vboxLayout= new QVBoxLayout();
    vboxLayout->addLayout(pSearchLayout);
    vboxLayout->addWidget(fileTreeview);
    //FileView
    QWidget *widget=new QWidget();
    widget->setLayout(vboxLayout);
    mUpLeftDock->setWidget(widget);
    //Database
    dispdata =new Dispdata();
    mDownLeftDock->setWidget(dispdata);

    //Excel
//    QWidget *upleftwidget =new QWidget(this);
//    QHBoxLayout *hlayout =new QHBoxLayout();
//    QVBoxLayout *vlayout =new QVBoxLayout();
//    QXlsx::Document xlsx("G:/计算机14-01.xlsx");
//    QXlsx::CellRange range=xlsx.dimension();
//    int rowcount=range.rowCount();
//    int columncount= range.columnCount();
//    qDebug()<<rowcount<<columncount;
//    m_excelWidget =new QTableWidget();
//    m_excelBtn=new QPushButton("生成excel");
//    connect(m_excelBtn,SIGNAL(clicked()),this,SLOT(slot_writeToExcel()));

//    hlayout->addStretch(10);
//    hlayout->addWidget(m_excelBtn);

//    vlayout->addWidget(m_excelWidget);
//    vlayout->addLayout(hlayout);
//    upleftwidget->setLayout(vlayout);
//    m_excelWidget->setRowCount(rowcount);
//    m_excelWidget->setColumnCount(columncount);
//    for(int i=0;i<rowcount;i++){
//        for(int j=0;j<columncount;j++){

//            QTableWidgetItem *item =new QTableWidgetItem(xlsx.read(i,j).toString());
//            item->setTextAlignment(Qt::AlignCenter);
//            m_excelWidget->setItem(i,j,item);
//        }
//    }
//    m_excelWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
//    m_excelWidget->setEditTriggers(QHeaderView::DoubleClicked);
//    m_excelWidget->setSelectionBehavior(QHeaderView::SelectRows);
//    m_excelWidget->horizontalHeader()->setVisible(true);
//    m_excelWidget->verticalHeader()->setVisible(true);
    mdiarea =new QMdiArea();
    mdiarea->setViewMode(QMdiArea::TabbedView);
    mdiarea->setTabPosition(QTabWidget::North);
    mdiarea->setTabsClosable(true);
    mdiarea->setTabsMovable(true);
    mdiarea->setTabShape(QTabWidget::Triangular);
    mUpRightDock->setWidget(mdiarea);

    //Browser
    Browser *browser =new Browser();
    mDownRightDock->setWidget(browser);

    hint_label= new QLabel();
    time_label= new QLabel();
    time_label->setMinimumSize(time_label->sizeHint());
    time_label->setAlignment(Qt::AlignCenter);
    hint_label->setText(tr("欢迎使用"));
    ui->statusBar->addWidget(hint_label,1);
    ui->statusBar->addWidget(time_label,0);
    ui->statusBar->setStyleSheet(QString("QStatusBar::item{border:0px}"));//去掉label的边框.

}

void MainWindow::slot_writeToExcel()
{
//    QXlsx::Document xlsx;
//    int rows =m_excelWidget->rowCount();
//    int cols= m_excelWidget->columnCount();
//    QString text;
//    for(int i=0;i<rows;i++){
//        for(int j=0;j<cols;j++){
//            text= m_excelWidget->item(i,j)->text();
//            xlsx.write(i+1,j+1,text);
//        }
//    }
//    xlsx.saveAs("G:/副本.xlsx");
}

void MainWindow::timerUpDate()//定时器更新时间的显示
{
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");//设置系统时间显示格式
    time_label->setText(str); //在标签上显示时间

}


void MainWindow::on_sel_sex(const QString &text)
{
    QString str;
    str = "You select " + text;
    QMessageBox::information(this, tr("Info"), str);
}

void MainWindow::search()
{
    QString strText = edit->text();
    if (!strText.isEmpty())
    {
        QMessageBox::information(this, "搜索", tr("搜索内容为%1").arg(strText));
    }
}

void MainWindow::setText(const QString &string)
{
    QString suffix =string.section(".",-1,-1);
    QTextEdit * textedit =new QTextEdit();

    QFile file(string);
    QTextStream in(&file);
    if(!file.open(QIODevice::ReadOnly)) return;

    if(suffix=="txt"||suffix=="ini"||suffix=="log"||suffix=="java"||suffix=="sql"||suffix=="xml")
    {
         QMdiSubWindow *child = mdiarea->addSubWindow(textedit);
         textedit->setText(in.readAll());
         child->setWindowTitle(string);
         child->show();
    }
    else if(suffix=="h"||suffix=="cpp"||suffix=="py")
    {
         QMdiSubWindow *child = mdiarea->addSubWindow(textedit);
         in.setCodec("utf-8");
         textedit->setText(in.readAll());
         child->setWindowTitle(string);
         child->show();
    }
    else if(suffix=="jpg"||suffix=="png")
    {
        QImage image(string);
        QLabel *label =new QLabel();
        image=image.scaled(label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        label->setPixmap(QPixmap::fromImage(image));
        label->setAlignment(Qt::AlignCenter);
        QMdiSubWindow *child = mdiarea->addSubWindow(label);
        child->setWindowTitle(string);
        child->show();
    }
    else if(suffix=="gif")
    {

        QLabel *label =new QLabel();
        QMovie *movie =new QMovie(string);
        label->setMovie(movie);
        label->setAlignment(Qt::AlignCenter);
        QMdiSubWindow *child =mdiarea->addSubWindow(label);
        child->setWindowTitle(string);
        child->show();
        movie->start();

    }
    else if(suffix=="mp3"||suffix=="wma")
    {

        mediaplayer->stop();
        mediaplayer->setMedia(QUrl::fromLocalFile(string));
        mediaplayer->play();
        QMessageBox::information(this,"正在播放",string);
    }

    return ;

}






void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else
        event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData =event->mimeData();
    if(mimeData->hasUrls())
    {

        QList<QUrl>urllist =mimeData->urls();
        if(urllist.isEmpty()) return;

        foreach (QUrl url, urllist)
        {
            QString fileName =url.toLocalFile();
            qDebug()<<fileName;
            if(!fileName.isEmpty())
            {
                QFile file(fileName);
                QString suffix =fileName.section(".",-1,-1);
                QTextEdit * textedit =new QTextEdit();
                QTextStream in(&file);
                if(!file.open(QIODevice::ReadOnly))return;

                if(suffix=="txt"||suffix=="ini"||suffix=="log"||suffix=="java"||suffix=="sql"||suffix=="xml")
                {
                    QMdiSubWindow *child = mdiarea->addSubWindow(textedit);
                    textedit->setText(in.readAll());
                    child->setWindowTitle(fileName);
                    child->show();
                }
                else if(suffix=="h"||suffix=="cpp"||suffix=="py")
                {
                    QMdiSubWindow *child = mdiarea->addSubWindow(textedit);
                    in.setCodec("utf-8");
                    textedit->setText(in.readAll());
                    child->setWindowTitle(fileName);
                    child->show();
                }
                else if(suffix=="jpg"||suffix=="png")
                {

                    QImage image(fileName);
                    QLabel *label =new QLabel();
                    image=image.scaled(label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
                    label->setPixmap(QPixmap::fromImage(image));
                    label->setAlignment(Qt::AlignCenter);
                    QMdiSubWindow *child = mdiarea->addSubWindow(label);
                    child->setWindowTitle(fileName);
                    child->show();

                }
                else if(suffix=="gif")
                {
                    qDebug()<<suffix;
                    QLabel *label =new QLabel();
                    QMovie *movie =new QMovie(fileName);
                    label->setMovie(movie);
                    label->setAlignment(Qt::AlignCenter);
                    QMdiSubWindow *child =mdiarea->addSubWindow(label);
                    child->setWindowTitle(fileName);
                    child->show();
                    movie->start();

                }
                else if(suffix=="mp3"||suffix=="wma")
                {
                    mediaplayer->stop();
                    mediaplayer->setMedia(QUrl::fromLocalFile(fileName));
                    mediaplayer->play();
                    QMessageBox::information(this,"正在播放",fileName);
                }

                else
                    return;

            }
        }
    }

}


