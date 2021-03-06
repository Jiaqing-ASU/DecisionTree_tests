#include "header.h"

// Parses a string and stores data into a vector of vector of strings
void parse(string& someString, vvs &attributeTable)
{
	int attributeCount = 0;
	vs vectorOfStrings;
	while (someString.length() != 0 && someString.find(',') != string::npos)
	{
		size_t pos;
		string singleAttribute;
		pos = someString.find_first_of(',');
		singleAttribute = someString.substr(0, pos);
		vectorOfStrings.push_back(singleAttribute);
		someString.erase(0, pos+1);
	}
	vectorOfStrings.push_back(someString);
	attributeTable.push_back(vectorOfStrings);
	vectorOfStrings.clear();
}
// Print tree
void printDecisionTree(node* nodePtr)
{
	if(nodePtr == NULL) {
		return;
	}
	if (!nodePtr->children.empty()) {
		cout << " Value: " << nodePtr->label << endl;
		cout << "Split on: " << nodePtr->splitOn;
		for (int i = 0; i < nodePtr->children.size(); i++) {   
			cout << "\t";
			printDecisionTree(nodePtr->children[i]);
		}
		return;
        } else {
		cout << "Predicted class = " << nodePtr->label;
        cout << endl;
		return;
	}
}

// Prunes a table based on a column/attribute's name and value of that attribute
// Removes that column and all rows that have that value for that column
vvi pruneTable(vvi &attributeTable, int &colName, int value)
{
	int i, j;
	vvi prunedTable;
	int column = -1;
	vi headerRow;
	for (i = 0; i < attributeTable[0].size(); i++) {
		if (attributeTable[0][i] == colName) {
			column = i;
			break;
		}
	}
	for (i = 0; i < attributeTable[0].size(); i++) {
		 if (i != column) {
		 	headerRow.push_back(attributeTable[0][i]);
		 }
	}
	prunedTable.push_back(headerRow);
	for (i = 0; i < attributeTable.size(); i++) {
		vi auxRow;
		if (attributeTable[i][column] == value) {
			for (j = 0; j < attributeTable[i].size(); j++) {
				if(j != column) {
					auxRow.push_back(attributeTable[i][j]);
				}
			}
			prunedTable.push_back(auxRow);
		}
	}
	return prunedTable;
}

// Recursively builds the decision tree based on the passed data and table info
node* buildDecisionTree(vvi &table, node* nodePtr, vvi &tableInfo)
{
	if (tableIsEmpty(table)) {
		return NULL;
	}
	if (isHomogeneous(table)) {
		nodePtr->isLeaf = true;
		nodePtr->label = table[1][table[1].size()-1];
		return nodePtr;
	} else {
		int splittingCol = decideSplittingColumn(table);
		nodePtr->splitOn = splittingCol;
		int colIndex = returnColumnIndex(splittingCol, tableInfo);
		int i;
		for (i = 1; i < tableInfo[colIndex].size(); i++) {
			node* newNode = (node*) new node;
			newNode->label = tableInfo[colIndex][i];
			nodePtr->childrenValues.push_back(tableInfo[colIndex][i]);
			newNode->isLeaf = false;
			newNode->splitOn = splittingCol;
			vvi auxTable = pruneTable(table, splittingCol, tableInfo[colIndex][i]);
			nodePtr->children.push_back(buildDecisionTree(auxTable, newNode, tableInfo));
		}
	}
	return nodePtr;
}

// Returns true if all rows in a subtable have the same class label
// This means that that node's class label has been decided
bool isHomogeneous(vvi &table)
{
	int i;
	int lastCol = table[0].size() - 1;
	int firstValue = table[1][lastCol];
	for (i = 1; i < table.size(); i++) {
		if (firstValue != table[i][lastCol]) {
			return false;
		}
	}
	return true;
}

// Returns a vector of integers containing the counts of all the various values of an attribute/column
vi countDistinct(vvi &table, int column)
{
	vi vectorOfInts;
	vi counts;
	bool found = false;
	int foundIndex;
	for (int i = 1; i < table.size(); i++) {
		for (int j = 0; j < vectorOfInts.size(); j++) {
			if (vectorOfInts[j] == table[i][column]) {
				found = true;
				foundIndex = j;
				break;
			} else {
				found = false;
			}
		}
		if (!found) {
			counts.push_back(1);
			vectorOfInts.push_back(table[i][column]);
		} else {
			counts[foundIndex]++;
		}
	}
	int sum = 0;
	for (int i = 0; i < counts.size(); i++) {
		sum += counts[i];
	}
	counts.push_back(sum);
	return counts;
}

// Decides which column to split on based on entropy
// Returns the column with the least entropy
int decideSplittingColumn(vvi &table)
{
	int column, i;
	double minEntropy = DBL_MAX;
	int splittingColumn = 0;
	vi entropies;
	for (column = 0; column < table[0].size() - 1; column++) {
		int colName = table[0][column];
		mii tempMap;
		vi counts = countDistinct(table, column);
		vd attributeEntropy;
		double columnEntropy = 0.0;
		for (i = 1; i < table.size()-1; i++) {
			double entropy = 0.0;
			if (tempMap.find(table[i][column]) != tempMap.end()) { 	// IF ATTRIBUTE IS ALREADY FOUND IN A COLUMN, UPDATE IT'S FREQUENCY
				tempMap[table[i][column]]++;
			} else { 							// IF ATTRIBUTE IS FOUND FOR THE FIRST TIME IN A COLUMN, THEN PROCESS IT AND CALCULATE IT'S ENTROPY
				tempMap[table[i][column]] = 1;
				vvi tempTable = pruneTable(table, colName, table[i][column]);
				vi classCounts = countDistinct(tempTable, tempTable[0].size()-1);
				int j, kkk;
				for (j = 0; j < classCounts.size(); j++) {
					double temp = (double) classCounts[j];
					entropy -= (temp/classCounts[classCounts.size()-1])*(log(temp/classCounts[classCounts.size()-1]) / log(2));
				}
				attributeEntropy.push_back(entropy);
				entropy = 0.0;
			}
		}
		for (i = 0; i < counts.size() - 1; i++) {
			columnEntropy += ((double) counts[i] * (double) attributeEntropy[i]);
		}
		columnEntropy = columnEntropy / ((double) counts[counts.size() - 1]);
		if (columnEntropy <= minEntropy) {
			minEntropy = columnEntropy;
			splittingColumn = column;
		}
	}
	return table[0][splittingColumn];
}

// Returns an integer which is the index of a column passed as a int
int returnColumnIndex(int &columnName, vvi &tableInfo)
{
	int i;
	for (i = 0; i < tableInfo.size(); i++) {
		if (tableInfo[i][0] == columnName) {
			return i;
		}
	}
	return -1;
}

// Returns true if the table is empty returns false otherwise
bool tableIsEmpty(vvi &table)
{
	return (table.size() == 1);
}

// Takes a row and traverses that row through the decision tree to find out the predicted class label. If none is found, returns the default class label which is the class label with the highest frequency
int testDataOnDecisionTree(vi &singleLine, node* nodePtr, vvi &tableInfo)
{
	int prediction;
	while (!nodePtr->isLeaf && !nodePtr->children.empty()) {
		int index = returnColumnIndex(nodePtr->splitOn, tableInfo);
		int value = singleLine[index];
		int childIndex = returnIndexOfVector(nodePtr->childrenValues, value);
		nodePtr = nodePtr->children[childIndex];
        if (nodePtr == NULL) {
			prediction = -1;
			break;
		}
		prediction = nodePtr->label;
	}
	return prediction;
}

// Returns an integer which is the index of an int in a vector of ints
int returnIndexOfVector(vi &intVector, int value)
{
	int i;
	for (i = 0; i < intVector.size(); i++) {
		if (intVector[i] == value)	{
			return i;
		}
	}
	return -1;
}

// Outputs the predictions to file and returns the accuracy of the classification
void printPredictions(vi &givenData, vi &predictions)
{
	ofstream outputFile;
	outputFile.open("decisionTreeOutput.txt");
	int correct = 0;
	outputFile << setw(3) << "#" << setw(16) << "Given Class" << setw(31) << right << "Predicted Class" << endl;
	outputFile << "--------------------------------------------------" << endl;
	for (int i = 0; i < givenData.size(); i++) {
		outputFile << setw(3) << i+1 << setw(16) << givenData[i];
		if (givenData[i] == predictions[i]) {
			correct++;
			outputFile << "  ------------  ";
		} else {
			outputFile << "  xxxxxxxxxxxx  ";
		}
		outputFile << predictions[i] << endl;
	}
	outputFile << "--------------------------------------------------" << endl;
	outputFile << "Total number of instances in test data = " << givenData.size() << endl;
	outputFile << "Number of correctly predicted instances = " << correct << endl;
	outputFile.close();
}

// Returns a vvi which contains information about the data table. The vvi contains the names of all the columns and the values that each column can take
vvi generateTableInfo(vvi &dataTable)
{
	vvi tableInfo;
	for (int i = 0; i < dataTable[0].size(); i++) {
		vi tempInfo;
		mii tempMap;
		for (int j = 0; j < dataTable.size(); j++) {
			if (tempMap.count(dataTable[j][i]) == 0) {
				tempMap[dataTable[j][i]] = 1;
				tempInfo.push_back(dataTable[j][i]);
			} else	{
				tempMap[dataTable[j][i]]++;
			}
		}
		tableInfo.push_back(tempInfo);
	}
	return tableInfo;
}