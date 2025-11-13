/**
VWAPer v0.1

// Read file in with the following format:
// [Stock],[Interval],[Volume Traded],[High],[Low]

// Calculate the total volume traded per Stock
// Calculate the total volume traded per Stock&Interval

// Write the total volume traded per Stock&Interval as a percentage of the total volume traded per Stock to stdout:
// [Stock],[Interval],[%Volume Traded]

// Write the delimiter '#' to stdout

// Write the maximum High and minimum Low for each Stock to stdout:
// [Stock],[Day High],[Day Low]

**/

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>
#include<unordered_map>

using namespace std;

#define DELIMITER "#"

class Stock {
private:
    string stockName; // name of stock
    size_t totalVolume; // volume is non-negative
    unordered_map<size_t, size_t> intervalVolume; // interval → volume
    double maxHigh, minLow; // maximum high, minimum low

public:
    // constructor: stockName, interval, volume, high, low
    Stock(const string& stockName, size_t interval, size_t volume, double high, double low)
        : stockName(stockName), totalVolume(volume), maxHigh(high), minLow(low)
    {
        intervalVolume[interval] = volume;
    }

    // updates stock info: interval, volume, high, low
    void updateStockInfo(size_t interval, size_t volume, double high, double low) {
        totalVolume += volume;
        intervalVolume[interval] += volume;
        maxHigh = max(maxHigh, high);
        minLow = min(minLow, low);
    }

    // returns stock name
    string getStockName() const { return stockName; }

    // returns total volume
    size_t getTotalVolume() const { return totalVolume; }

    // returns volume at a specific interval
    size_t getVolumeAtInterval(const size_t interval) const {
        auto itr = intervalVolume.find(interval);
        if (itr == intervalVolume.end()) {
            cerr << "Warning: interval " << interval << " not found for this stock " << endl;
            return 0;
        }
        return itr->second;
    }

    // returns maximum high
    double getMaxHigh() const { return maxHigh; }

    // returns minimum low
    double getMinLow() const { return minLow; }

    // returns reference to interval-volume map
    const unordered_map<size_t, size_t>& getIntervalVolume() const { return intervalVolume; }
};

class MarketData {
private:
    unordered_map<string, Stock> allStocks; // stores all Stock objects keyed by their stockName
    vector<string> stockOrder; // keeps track of order stocks were added
    size_t intervals = 0; // highest interval number across all stocks

public:
    // check if a stock exists
    bool found(const string& stockName){
        return allStocks.find(stockName) != allStocks.end();
    }

    // retrieve a pointer to a stock object
    Stock* getStock(const string& stockName) {
        auto itr = allStocks.find(stockName);
        if (itr == allStocks.end()) {
            cerr << "Warning: Stock '" << stockName << "' not found in records." << endl;
            return nullptr;
        }
        return &itr->second;
    }

    // add a new stock to the market
    void addStock(const string& stockName, size_t interval, size_t volume, double high, double low) {
        if (allStocks.find(stockName) != allStocks.end()) {
            cerr << "Warning: Stock '" << stockName << "' already exists." << endl;
            return;
        }
        allStocks.emplace(stockName, Stock(stockName, interval, volume, high, low));
        stockOrder.push_back(stockName);
        intervals = max(intervals, interval); // track maximum interval seen
    }

    // update an existing stock with new interval data
    void updateStock(const string& stockName, size_t interval, size_t volume, double high, double low) {
        auto itr = allStocks.find(stockName);
        if(itr == allStocks.end()){
            cerr << "Error: Cannot update non-existent stock '" << stockName << "'." << endl;
            return;
        }
        itr->second.updateStockInfo(interval, volume, high, low);
        intervals = max(intervals, interval); // update maximum interval if needed
    }

    // get all stocks
    const unordered_map<string, Stock>& getAllStocks() const { return allStocks; }

    // get the order in which stocks were added
    const vector<string>& getStockorder() const { return stockOrder; }

    // get the highest interval number seen
    size_t getNumOfIntervals() const { return intervals; }
};

void outputRequirements(MarketData &marketData){
    const unordered_map<string, Stock>& currentMarket = marketData.getAllStocks(); // all stocks in marketData
    const vector<string>& currentStockOrder = marketData.getStockorder(); // order of stocks
    const size_t intervals = marketData.getNumOfIntervals(); // total intervals 

    // outputs stock name, interval of stock, percentage of total volume traded per stock
    for(size_t i = 1; i <= intervals; i++){ 

         for(const string &stockName: currentStockOrder){

            const Stock * currentStock = marketData.getStock(stockName);
            size_t totalVolume = currentStock->getTotalVolume();
            double percentageOfVolumeTraded = (double(currentStock->getVolumeAtInterval(i)) / totalVolume) * 100;

            cout << currentStock->getStockName() << "," << i << "," <<  percentageOfVolumeTraded << endl;
         }
    }

    cout << DELIMITER << endl;

    // outputs stock name, maximum high, and minimum low for each stock
    for(const string &stockName: currentStockOrder){
        const Stock * currentStock = marketData.getStock(stockName);
        cout << stockName << "," << currentStock->getMaxHigh() << "," << currentStock->getMinLow() << endl;
    }
}

MarketData readMarketData(const string& filename) {
    MarketData marketData; // create MarketData object
    ifstream file(filename); // open input file
    if (!file) {
        throw runtime_error("Could not open file: " + filename); // error if file can't open
    }

    string line;
    while (getline(file, line)) { // read file line by line
        istringstream iss(line); 
        string stockName;
        size_t interval, volume;
        double high, low;
        if (!(iss >> stockName >> interval >> volume >> high >> low)) {
            cerr << "Error parsing line: " << line << endl; // skip invalid lines
            continue;
        }

        if (marketData.found(stockName)) {
            marketData.updateStock(stockName, interval, volume, high, low); // update existing stock
        } else {
            marketData.addStock(stockName, interval, volume, high, low); // add new stock
        }
    }

    return marketData; // return populated MarketData
}

int main() {
    try {
        MarketData marketData = readMarketData("data/market.txt");
        outputRequirements(marketData);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

/*

EXPECTED OUTPUT:        | ACTUAL OUTPUT:
                        | 
VOD.L,1,80              | VOD.L,1,80
BT.LN,1,25              | BT.LN,1,25
VOD.L,2,20              | VOD.L,2,20
BT.LN,2,75              | BT.LN,2,75
#                       | #
VOD.L,184.1,182.4       | VOD.L,184.1,182.4
BT.LN,449.8,448.2       | BT.LN,449.8,448.2

assumptions made:
1. stocks share a max interval, all stocks have the same interval
2. interval increments by 1 given a new interval
3. string of stock name sizes are unknown. total volume and interval are non-negative integers, stored as size_t. high and low are doubles.

my own points of consideration:
- reading and updating marketData is O(n) for n lines in the file.
- each addStock or updateStock is O(1) average (unordered_map insert/access).
- generating output is O(S * I) for S stocks and I intervals.

- separate functions and classes into different files, main file should be stand-alone too.



part c: How would you check this code works as intended?

I would check the code works as intended by writing automated tests using a framework like google test. I would
create unit tests for each class and function, verifying that they return the correct values, handle edge cases,
and correctly process sample inputs to produce the expected outputs. for example, I would test the stock class to 
ensure that updating intervals and calculating total volume works correctly, and that retrieving maximum and 
minimum prices returns the expected values. I would also write tests for marketdata to confirm that adding and
updating stocks behaves as expected and that the correct number of intervals is tracked. additionally, I would
include tests for the output generation, feeding in small sample datasets and comparing the actual output to the
expected output, including edge cases like missing intervals or zero volumes. this approach ensures I can confidently
verify that every part of the program produces the correct results under a wide range of conditions.
*/
