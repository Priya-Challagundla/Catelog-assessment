// Name: Ganta Venkata Kousik
// Roll No: 21pa1a0549

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

// Function to decode a string in a given base to a long long integer
long long decodeValue(const string& encodedValue, int base) {
    return stoll(encodedValue, nullptr, base);
}

// Function to perform Gaussian elimination on a matrix
vector<double> solveUsingGaussianElimination(vector<vector<double>> augmentedMatrix, vector<double> constantTerms) {
    int numRows = augmentedMatrix.size();
    for (int pivotRow = 0; pivotRow < numRows; ++pivotRow) {
        // Normalize the pivot row
        double pivotElement = augmentedMatrix[pivotRow][pivotRow];
        for (int col = 0; col < numRows; ++col) {
            augmentedMatrix[pivotRow][col] /= pivotElement;
        }
        constantTerms[pivotRow] /= pivotElement;

        // Eliminate the current column in other rows
        for (int row = 0; row < numRows; ++row) {
            if (row != pivotRow) {
                double factor = augmentedMatrix[row][pivotRow];
                for (int col = 0; col < numRows; ++col) {
                    augmentedMatrix[row][col] -= factor * augmentedMatrix[pivotRow][col];
                }
                constantTerms[row] -= factor * constantTerms[pivotRow];
            }
        }
    }
    return constantTerms; // The constantTerms now contains the solution
}

void processPolynomialTestCase(const char* fileName) {
    // Open the JSON file containing the encoded polynomial data
    FILE* filePointer = fopen(fileName, "r");
    if (!filePointer) {
        cerr << "Error: Unable to open " << fileName << endl;
        return;
    }

    // Read and parse the JSON file
    char buffer[65536];
    FileReadStream inputStream(filePointer, buffer, sizeof(buffer));
    Document jsonDocument;
    jsonDocument.ParseStream(inputStream);
    fclose(filePointer);

    // Extract the number of points and the polynomial degree
    const Value& polynomialKeys = jsonDocument["keys"];
    int numberOfPoints = polynomialKeys["n"].GetInt();
    int polynomialOrder = polynomialKeys["k"].GetInt();

    // Initialize the augmented matrix and constants vector
    vector<vector<double>> augmentedMatrix(numberOfPoints, vector<double>(polynomialOrder, 0));
    vector<double> constantTerms(numberOfPoints, 0);

    // Populate the matrix and vector with decoded values
    for (int pointIndex = 1; pointIndex <= numberOfPoints; ++pointIndex) {
        string pointKey = to_string(pointIndex);
        int xCoordinate = pointIndex;
        const Value& pointData = jsonDocument[pointKey.c_str()];
        int base = pointData["base"].GetInt();
        string encodedYValue = pointData["value"].GetString();
        long long decodedYValue = decodeValue(encodedYValue, base);
        constantTerms[pointIndex - 1] = decodedYValue;
        for (int power = 0; power < polynomialOrder; ++power) {
            augmentedMatrix[pointIndex - 1][power] = pow(xCoordinate, polynomialOrder - power - 1);
        }
    }

    // Solve the system using Gaussian elimination
    vector<double> polynomialCoefficients = solveUsingGaussianElimination(augmentedMatrix, constantTerms);

    // Output the constant term, which is the last element in the polynomial coefficients
    cout << "The secret (constant term c) for " << fileName << " is: " << polynomialCoefficients.back() << endl;
}

int main() {
    // Process each test case
    processPolynomialTestCase("input1.json");
    processPolynomialTestCase("input2.json");

    return 0;
}
