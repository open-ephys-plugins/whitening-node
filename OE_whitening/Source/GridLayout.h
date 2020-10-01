#pragma once
#include <EditorHeaders.h>

// A simple grid layout class to make arranging widget in earlier version of JUCE easier

class GridLayout {
private:
	Rectangle<int> m_area;
	int padding = 5;
	int itemHeight;
	int itemWidth;
	int titleHeight=10;
	int footerHeight = 10;
	int m_ncol;
	int m_nrow;

public:
	GridLayout(Rectangle<int> area, int nrow, int ncol);
	void set_padding(int padding);
	Rectangle<int> getBoundAt(int row, int col);

};