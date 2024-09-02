#include "leespoiler.h"

#include <QPropertyAnimation>
#include <QDebug>

LeeSpoiler::LeeSpoiler(const QString& title, const int animationDuration, QWidget* parent)
    : QWidget(parent), animationDuration(animationDuration)
{
    toggleButton = new QToolButton(this);
    headerLine = new QFrame(this);
    toggleAnimation = new QParallelAnimationGroup(this);
    contentArea = new QScrollArea(this);
    mainLayout = new QGridLayout(this);

    UnfoldDirection = Qt::ArrowType::UpArrow;
    foldDirection = Qt::ArrowType::DownArrow;

    toggleButton->setStyleSheet("QToolButton {border: none;}");
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton->setArrowType(foldDirection);
    toggleButton->setText(title);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);

    headerLine->setFrameShape(QFrame::HLine);
    headerLine->setFrameShadow(QFrame::Sunken);
    headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // start out collapsed
    contentArea->setMaximumHeight(0);
    contentArea->setMinimumHeight(0);

    // let the entire widget grow and shrink with its content
    toggleAnimation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation->addAnimation(new QPropertyAnimation(contentArea, "maximumHeight"));

    mainLayout->setVerticalSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    int row = 0;
    mainLayout->addWidget(toggleButton, row, 0, 1, 1, Qt::AlignCenter);
    mainLayout->addWidget(headerLine, row++, 2, 1, 1);
    mainLayout->addWidget(contentArea, row, 0, 1, 3);
    setLayout(mainLayout);

    connect(toggleButton, &QToolButton::toggled, this, &LeeSpoiler::toggle);

}

void LeeSpoiler::toggle(bool expanded)
{
    toggleButton->setArrowType(expanded ? UnfoldDirection : foldDirection);
    toggleAnimation->setDirection(expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation->start();

    this->isExpanded = expanded;

    //qDebug() << "MV: toggle: isExpanded " << isExpanded;
}

void LeeSpoiler::setContentLayout(QLayout& contentLayout)
{
    delete contentArea->layout();
    contentArea->setLayout(&contentLayout);
    collapsedHeight = sizeHint().height() - contentArea->maximumHeight();

    collapsedWidth = sizeHint().width() - contentArea->maximumWidth();
    updateHeightsWidth();
}

void LeeSpoiler::setTitle(QString title){
    toggleButton->setText(std::move(title));
}

void LeeSpoiler::updateHeightsWidth(bool isWeight)
{
    int contentHeight = !isWeight ? contentArea->layout()->sizeHint().height() :
                            contentArea->layout()->sizeHint().width();

    int contentWidth = contentArea->layout()->sizeHint().width();

    for (int i = 0; i < toggleAnimation->animationCount() - 1; ++i)
    {
        QPropertyAnimation* SectionAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(i));
        SectionAnimation->setDuration(animationDuration);
        SectionAnimation->setStartValue(collapsedHeight);
        SectionAnimation->setEndValue(collapsedHeight + contentHeight);
    }

    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(toggleAnimation->animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);

    toggleAnimation->setDirection(isExpanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation->start();
}

void LeeSpoiler::SetDirection(const Qt::ArrowType inType, bool isUnfold)
{
    isUnfold == false ? foldDirection = inType : UnfoldDirection = inType;
}

