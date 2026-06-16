#pragma once

#include <QMainWindow>

class MainWindow : public QMainWindow{
    
    Q_OBJECT

public:
    explicit MainWindow(QWidget* _parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUI();
    void setupLayout();    
};