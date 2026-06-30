#include <iostream>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QRandomGenerator>
#include <QWidget>
#include <cmath>

using namespace std;

bool isFirstClick = true;
int boardSize;
int score = 0;
QGraphicsTextItem *Score = nullptr;

int firstClickRow = -1;
int firstClickCol = -1;

QGraphicsScene* generateRandomGrid(QGraphicsScene *scene, int rows, int cols, int cellSize, int gridSize){
    QPen pen(Qt::black, 1);

    int colours_needed = gridSize - 1;
    
    QVector<QColor> colourPalette;
    colourPalette.reserve(colours_needed);
    
    for(int i = 0; i < colours_needed; i++){
      int hue = (360*i)/colours_needed;
      QColor colour = QColor::fromHsv(hue,220,240);
      colourPalette.append(colour);
    }
	    
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++){
            // Allocate canvas positions
            qreal x = c * cellSize;
            qreal y = r * cellSize;

            QGraphicsRectItem *cell = new QGraphicsRectItem(0, 0, cellSize, cellSize);
            cell->setPos(x, y);
	    cell->setData(0,r);
	    cell->setData(1,c);
	    
	    int random_index = QRandomGenerator::global()->bounded(colours_needed);
	    QColor cell_colour = colourPalette[random_index];
	    
	    cell->setBrush(QBrush(cell_colour));
	    cell->setPen(pen);
	    
            scene->addItem(cell);
        }
    }

    return scene;
}

QColor getCellColour(QGraphicsScene *scene, int r, int c){
  for(QGraphicsItem *item : scene->items()){
    if(item->data(0).isValid() && item->data(1).isValid()){
      if(item->data(0).toInt() == r && item->data(1).toInt() == c){
	QGraphicsRectItem *rect = static_cast<QGraphicsRectItem*>(item);
	return rect->brush().color();
      }
    }
  }
  return QColor();
}

QGraphicsScene* updateGrid(QGraphicsScene *scene, int boardSize, int row1, int col1, int row2, int col2){
    int colours_needed = boardSize - 1;
    if(colours_needed < 1){
	colours_needed = 1;
      }
    
    QVector<QColor> colourPalette;
    colourPalette.reserve(colours_needed);
    for(int i = 0; i < colours_needed; i++){
      int hue = (360 * i) / colours_needed;
      colourPalette.append(QColor::fromHsv(hue, 220, 240));
    }

    int checkRows[2] = {row1, row2};
    int checkCols[2] = {col1, col2};

    QVector<QVector<bool>> matchMap(boardSize, QVector<bool>(boardSize, false));

    // Array of 4-directional offsets: {rowOffset, colOffset} -> Right, Left, Down, Up
    int dr[4] = {0, 0, 1, -1};
    int dc[4] = {1, -1, 0, 0};

    // 1. DYNAMICALLY SCAN ALL DIRECTIONS
    for(int i = 0; i < 2; i++){
      int startR = checkRows[i];
      int startC = checkCols[i];
      QColor targetColor = getCellColour(scene, startR, startC);
      if(!targetColor.isValid()){
	continue;
      }
      
      // Check Horizontal (Directions 0 and 1: Right and Left)
      int horizontalCount = 1;
      for(int dir = 0; dir < 2; dir++){
          int r = startR + dr[dir];
          int c = startC + dc[dir];
          while(r >= 0 && r < boardSize && c >= 0 && c < boardSize && getCellColour(scene, r, c) == targetColor){
              horizontalCount++;
              r += dr[dir];
              c += dc[dir];
          }
      }

      // If horizontal line is 3 or more, trace again to mark ALL of them in our matchMap
      if(horizontalCount >= 3){
          matchMap[startR][startC] = true;
          for(int dir = 0; dir < 2; dir++){
              int r = startR + dr[dir];
              int c = startC + dc[dir];
              while(r >= 0 && r < boardSize && c >= 0 && c < boardSize && getCellColour(scene, r, c) == targetColor){
                  matchMap[r][c] = true;
                  r += dr[dir];
                  c += dc[dir];
              }
          }
      }

      // Check Vertical (Directions 2 and 3: Down and Up)
      int verticalCount = 1;
      for(int dir = 2; dir < 4; dir++){
          int r = startR + dr[dir];
          int c = startC + dc[dir];
          while(r >= 0 && r < boardSize && c >= 0 && c < boardSize && getCellColour(scene, r, c) == targetColor){
              verticalCount++;
              r += dr[dir];
              c += dc[dir];
          }
      }

      // If vertical line is 3 or more, mark ALL of them in our matchMap
      if(verticalCount >= 3){
          matchMap[startR][startC] = true;
          for(int dir = 2; dir < 4; dir++){
              int r = startR + dr[dir];
              int c = startC + dc[dir];
              while(r >= 0 && r < boardSize && c >= 0 && c < boardSize && getCellColour(scene, r, c) == targetColor){
                  matchMap[r][c] = true;
                  r += dr[dir];
                  c += dc[dir];
              }
          }
      }
    }

    // 2. RE-ROLL RANDOM BRUSH FILLS FOR ALL MARKED ITEMS
    for(QGraphicsItem *item : scene->items()){
      if(item->data(0).isValid() && item->data(1).isValid()){
	  int r = item->data(0).toInt();
	  int c = item->data(1).toInt();

	  if(matchMap[r][c]){
	     QGraphicsRectItem *rect = static_cast<QGraphicsRectItem*>(item);
             int random_index = QRandomGenerator::global()->bounded(colours_needed);
             QColor new_colour = colourPalette[random_index];
             rect->setBrush(QBrush(new_colour));
	  }
	}
    }
    
    return scene;
}

bool hasthreeormore(QGraphicsScene *scene, int gridSize, int r1, int c1, int r2, int c2) {
    // We only need to check two specific positions: (r1, c1) and (r2, c2)
    int rows[2] = {r1, r2};
    int cols[2] = {c1, c2};

    for (int i = 0; i < 2; i++) {
        int r = rows[i];
        int c = cols[i];
        
        QColor current = getCellColour(scene, r, c);
        if(!current.isValid()){
	  continue;
	}
	
        // --- 1. HORIZONTAL MATCH CHECKS ---
        // Case A: Swapped cell is the LEFT anchor ([X] [ ] [ ])
        if(c + 2 < gridSize){
            if(getCellColour(scene, r, c + 1) == current && getCellColour(scene, r, c + 2) == current){
                return true;
            }
        }
        // Case B: Swapped cell is the MIDDLE anchor ([ ] [X] [ ])
        if (c - 1 >= 0 && c + 1 < gridSize) {
            if (getCellColour(scene, r, c - 1) == current && getCellColour(scene, r, c + 1) == current) {
                return true;
            }
        }
        // Case C: Swapped cell is the RIGHT anchor ([ ] [ ] [X])
        if(c - 2 >= 0){
            if(getCellColour(scene, r, c - 1) == current && getCellColour(scene, r, c - 2) == current){
                return true;
            }
        }

        // --- 2. VERTICAL MATCH CHECKS ---
        // Case A: Swapped cell is the TOP anchor
        if(r + 2 < gridSize){
            if(getCellColour(scene, r + 1, c) == current && getCellColour(scene, r + 2, c) == current){
                return true;
            }
        }
        // Case B: Swapped cell is the MIDDLE anchor
        if(r - 1 >= 0 && r + 1 < gridSize){
            if(getCellColour(scene, r - 1, c) == current && getCellColour(scene, r + 1, c) == current){
                return true;
            }
        }
        // Case C: Swapped cell is the BOTTOM anchor
        if(r - 2 >= 0){
            if(getCellColour(scene, r - 1, c) == current && getCellColour(scene, r - 2, c) == current){
                return true;
            }
        }
    }

    return false; 
}

// Returns true if swapping (r1,c1) with (r2,c2) results in a valid match
bool isValidMoveSimulation(QGraphicsScene *scene, int gridSize, int r1, int c1, int r2, int c2){
    // Only simulate neighbors
    if((abs(r1 - r2) == 1 && abs(c1 - c2) == 0) || ((abs(r1 - r2) == 0 && abs(c1 - c2) == 1))){
        QGraphicsRectItem *cell1 = nullptr;
        QGraphicsRectItem *cell2 = nullptr;

        for(QGraphicsItem *item : scene->items()){
	  if(!item->data(0).isValid()){
	    continue;
	  }
	  
            int r = item->data(0).toInt();
            int c = item->data(1).toInt();

            if(r == r1 && c == c1){
	      cell1 = static_cast<QGraphicsRectItem*>(item);
	    }
	    
            if(r == r2 && c == c2){
	      cell2 = static_cast<QGraphicsRectItem*>(item);
        }
	}
	
        if(cell1 && cell2){
            QBrush brushA = cell1->brush();
            QBrush brushB = cell2->brush();

            // 1. Temporarily swap the colors
            cell1->setBrush(brushB);
            cell2->setBrush(brushA);

            // 2. Check if this swap creates a match
            bool formsMatch = hasthreeormore(scene, gridSize, r1, c1, r2, c2);

            // 3. Immediately revert back to keep the board clean
            cell1->setBrush(brushA);
            cell2->setBrush(brushB);

            return formsMatch;
        }
    }
    return false;
}

// Scans the entire board to see if ANY valid move exists
bool hasAnyMovesLeft(QGraphicsScene *scene, int gridSize) {
    for(int r = 0; r < gridSize; r++){
        for(int c = 0; c < gridSize; c++){
            // We only need to check checking DOWN and RIGHT for every cell 
            // to avoid testing the same pairs twice.
	  if(r + 1 < gridSize){
	    if(isValidMoveSimulation(scene, gridSize, r, c, r + 1, c)){
	      return true;
            }
	}
	
            if(c + 1 < gridSize){
	      if(isValidMoveSimulation(scene, gridSize, r, c, r, c + 1)){
		return true;
            }
	    }
        }
    }
    return false; // Loop finished and found zero valid moves
}

void swap(QGraphicsScene *scene, int row1, int col1, int row2, int col2, int gridSize){
      
  if(row1 == row2 && col1 == col2){
    for(QGraphicsItem *item : scene->items()){
      if(item->data(0).isValid() && item->data(0).toInt() == row1 && item->data(1).toInt() == col1){
	item->setOpacity(1.0);
	break;
      }
    }
    return;
  }
  
  QGraphicsRectItem *originCell = nullptr;
  QGraphicsRectItem *targetCell = nullptr;

  for(QGraphicsItem *item : scene->items()){
    if(!item->data(0).isValid()){
      continue;
    }

    int r = item->data(0).toInt();
    int c = item->data(1).toInt();

    if(r == row1 && c == col1){
      originCell = static_cast<QGraphicsRectItem*>(item);
    }

    if(r == row2 && c == col2){
      targetCell = static_cast<QGraphicsRectItem*>(item);
    }
  }

  if(originCell){
    originCell->setOpacity(1.0);
  }
  
  if(targetCell){
    targetCell->setOpacity(1.0);
  }
  
  if(originCell && targetCell){
    
    if((abs(row1-row2) == 1 && abs(col1-col2) == 0) || ((abs(row1-row2) == 0 && abs(col1-col2) == 1))){

      QBrush brushA = originCell->brush();
      QBrush brushB = targetCell->brush();

      originCell->setBrush(brushB);
      targetCell->setBrush(brushA);
      
    if(hasthreeormore(scene,boardSize,row1,col1,row2,col2)){
          bool has_matches = true;

    while(has_matches){
      has_matches = false;
      
      for(int row = 0; row < boardSize; row++){
	for(int col = 0; col < boardSize; col++){
	  if(hasthreeormore(scene,boardSize,row,col,row,col)){
	    score++;
	    Score->setPlainText("Score: " + QString::number(score));
	    Score->setPos(20*boardSize,-60);
	    updateGrid(scene,boardSize,row,col,row,col);

	    has_matches = true;
	}
      }
    }
    }
     }
    
    else{
      originCell->setBrush(brushA);
      targetCell->setBrush(brushB);
    }
}
}

  if(!hasAnyMovesLeft(scene, boardSize)){
    Score->setPlainText("GAME OVER! No more moves left! Final Score: " + QString::number(score));
    Score->setPos(0,-60);
}
}


class ClickFilter : public QObject{
  QGraphicsScene *m_scene;
public:
  ClickFilter(QGraphicsScene *scene) : m_scene(scene){}
protected:
bool eventFilter(QObject *watched, QEvent *event) override{
  if(watched == m_scene && event->type() == QEvent::GraphicsSceneMousePress){
  auto *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
    QPointF clickedPos = mouseEvent->scenePos();
    QGraphicsItem *clickedItem = m_scene->itemAt(clickedPos, QTransform());

    if(clickedItem && clickedItem->data(0).isValid()){
      int cur_row = clickedItem->data(0).toInt();
      int cur_col = clickedItem->data(1).toInt();

      if(isFirstClick){
	firstClickRow = cur_row;
	firstClickCol = cur_col;
	isFirstClick = false;
	clickedItem->setOpacity(0.5);
      }
      
      else{
	int secondClickRow = clickedItem->data(0).toInt();
	int secondClickCol = clickedItem->data(1).toInt();

	isFirstClick = true;
	clickedItem->setOpacity(1.0);
	
	swap(m_scene,firstClickRow,firstClickCol,secondClickRow,secondClickCol,boardSize);
      }
    }
    return true;
  }
  return QObject::eventFilter(watched,event);
}
};
  
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    
    QGraphicsScene *scene = new QGraphicsScene();

    cout << "What size board you want to play with?\n";
    cin >> boardSize;

    QString score_text = "Score: " + QString::number(score);
    Score = scene->addText(score_text, QFont("Arial", 25));
    Score->setPos(20*boardSize,-60);
    
    generateRandomGrid(scene, boardSize, boardSize, 50, boardSize);

    bool has_matches = true;

    while(has_matches){
      has_matches = false;
      
      for(int row = 0; row < boardSize; row++){
	for(int col = 0; col < boardSize; col++){
	  if(hasthreeormore(scene,boardSize,row,col,row,col)){
	    score++;
	    Score->setPlainText("Score: " + QString::number(score));
	    Score->setPos(20*boardSize,-60);
	    updateGrid(scene,boardSize,row,col,row,col);

	    has_matches = true;
	}
      } 
    }
    }
      
    QGraphicsView *view = new QGraphicsView(scene);
    view->resize(60*boardSize, 60*boardSize);
    view->setWindowTitle("Bejeweled");
    view->show();

    ClickFilter *filter = new ClickFilter(scene);
    scene->installEventFilter(filter);
    
    return app.exec();
}
