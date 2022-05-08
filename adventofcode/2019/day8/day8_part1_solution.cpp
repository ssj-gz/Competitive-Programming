#include <iostream>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    constexpr int imageWidth = 25;
    constexpr int imageheight = 6;
    constexpr int totalPixelsInLayer = imageWidth * imageheight;

    string imageInfo;
    cin >> imageInfo;


    int fewest0sInLayer = numeric_limits<int>::max();
    int result = -1;
    while (!imageInfo.empty())
    {
        const string layerPixels(imageInfo.begin(), imageInfo.begin() + totalPixelsInLayer);
        imageInfo.erase(imageInfo.begin(), imageInfo.begin() + totalPixelsInLayer);
        const int num0sInLayer = static_cast<int>(count(layerPixels.begin(), layerPixels.end(), '0'));
        if (num0sInLayer < fewest0sInLayer)
        {
            fewest0sInLayer = num0sInLayer;

            const int num1sInLayer = static_cast<int>(count(layerPixels.begin(), layerPixels.end(), '1'));
            const int num2sInLayer = static_cast<int>(count(layerPixels.begin(), layerPixels.end(), '2'));
            result = num1sInLayer * num2sInLayer;
        }
    }
    assert(result != -1);
    cout << result << endl;
}
