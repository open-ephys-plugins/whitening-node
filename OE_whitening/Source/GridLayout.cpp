#include "GridLayout.h"

GridLayout::GridLayout(Rectangle<int> area, int nrow, int ncol)
{
	m_area = area;
	m_nrow = nrow;
	m_ncol = ncol;
	itemWidth = area.getWidth() / ncol - 2*padding;
	itemHeight = (area.getHeight()-titleHeight-footerHeight) / nrow - 2*padding;
	
}

void GridLayout::set_padding(int padding) {
	padding = padding;
}

Rectangle<int> GridLayout::getBoundAt(int row, int col)
{
	//Return the area specified by the row and col
	int x = col * (m_area.getWidth() / m_ncol) +padding;
	int y = row * ((m_area.getHeight()-titleHeight)/m_nrow) + padding + titleHeight;
	int width = itemWidth;
	int height = itemHeight;

	return Rectangle<int>(x,y,width,height);
}
