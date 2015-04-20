#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ElfDataType.h"
#include <stdio.h>
#include <elf.h>
#include <QMessageBox>
#include "ElfHeaderClass.h"
#include <QList>
#include <QTreeWidgetItem>
#define Col_Offset 0
#define Col_Member 1
#define Col_Value 2
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    char* bufferPoint;
    int bitInfo;
    try
    {
        bitInfo = ReadFile("/home/red/a.out", bufferPoint);
    } catch(...){
        QMessageBox::information(this, "File open error", "File open error");
    }

    switch(bitInfo)
    {
      case ELFCLASS32:
        //QMessageBox::information(this, "32bit ELF", "this is 32bit elf");
        break;
      case ELFCLASS64:
        //QMessageBox::information(this, "64bit ELF", "this is 64bit elf");
        break;
      default:
        //QMessageBox::information(this, "wrong ELF", "this is wrong elf");
        break;
    }

    //ElfHeaderClass<Elf32_Ehdr> elfHeader(new Elf32_Ehdr);
    ElfHeaderClass<Elf32_Ehdr> elfHeader(bitInfo);
    elfHeader.SetHeader(bufferPoint);
    elfHeader.PrintHeader();

    ui->treeWidgetName->setColumnCount(1);
    ui->treeWidgetName->setHeaderLabel("NAME");

    ui->treeWidgetValue->setColumnCount(3);
    ui->treeWidgetValue->setHeaderLabels(QStringList() << "Offset" << "Member" << "Value");

    AddRootNameWidget(ui->treeWidgetName, "Header");
    foreach(ElfDataType item, elfHeader.headerMemberList)
    {
        AddRootValueWidget(ui->treeWidgetValue, item.memberOffset,  item.memberName, item.memberValue);
    }


    //ElfPrgHeaderClass<Elf32_Phdr> elfPrgHeader;
    //elfPrgHeader.SetHeader(bufferPoint, elfHeader.prgHeaderOffset, elfHeader.prgHeaderEntSize, elfHeader.prgHeaderNumber);
    //elfPrgHeader.PrintHeader();

    //ElfSeHeaderClass<Elf32_Shdr> elfSeHeader;
    //elfSeHeader.SetHeader(bufferPoint, elfHeader.seHeaderOffset, elfHeader.seHeaderEntSize, elfHeader.seHeaderNumber);
    //elfSeHeader.PrintHeader();


    free(bufferPoint);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::ReadFile(char* filename, char* &bufferPoint)
{
  FILE* fp;
  fp = fopen(filename,"rb");
  if(fp==NULL)
  {
    fputs("File error", stderr);
    exit(1);
  }
//GET FILE SIZE
  size_t fileSize;
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  rewind(fp);


  bufferPoint = (char*)malloc(sizeof(char)*fileSize);
//READ FILE
  size_t result;
  result = fread(bufferPoint,1,fileSize,fp);
  if(result != fileSize)
  {
    fputs("Reading error", stderr);
    exit(3);
  }
//DUMP FILE

  unsigned char e_ident[EI_NIDENT];
  memcpy((char*)&e_ident, bufferPoint, EI_NIDENT);

  int bitInfo = e_ident[EI_CLASS];



  //파일 헥스 덤프
  //dumpcode((unsigned char*)buffer,fileSize);

  fclose(fp);
  return bitInfo;
}
void MainWindow::AddRootNameWidget(QTreeWidget* view, QString name)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(view);
    item->setText(0, name);
    view->addTopLevelItem(item);
}

void MainWindow::AddRootValueWidget(QTreeWidget* view, QString offset, QString member, QString value)
{

    QTreeWidgetItem *item = new QTreeWidgetItem(view);

    item->setText(Col_Offset,offset);
    item->setText(Col_Member, member);
    item->setText(Col_Value,value);
    view->addTopLevelItem(item);

}

void MainWindow::AddChild(int column, QTreeWidgetItem* parent, QString name)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(column,name);
    parent->addChild(item);
}