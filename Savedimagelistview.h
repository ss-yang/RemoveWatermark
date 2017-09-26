#ifndef SAVEDIMAGELISTVIEW_H
#define SAVEDIMAGELISTVIEW_H

#include <QListView>

class SavedImageListView : public QListView
{
//    Q_OBJECT
public:
    SavedImageListView(QWidget *parent=0);
    ~SavedImageListView();
};

#endif // SAVEDIMAGELISTVIEW_H
