#include <iostream>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    constexpr int imageWidth = 25;
    constexpr int imageHeight = 6;
    constexpr int totalPixelsInLayer = imageWidth * imageHeight;

    string imageInfo;
    cin >> imageInfo;

    vector<vector<int>> decodedImage(imageWidth, vector<int>(imageHeight, - 1));

    while (!imageInfo.empty())
    {
        const string layerPixels(imageInfo.begin(), imageInfo.begin() + totalPixelsInLayer);
        imageInfo.erase(imageInfo.begin(), imageInfo.begin() + totalPixelsInLayer);
        for (int x = 0; x < imageWidth; x++)
        {
            for (int y = 0; y < imageHeight; y++)
            {
                const auto pixelInLayer = layerPixels[x + y * imageWidth] - '0';
                if (pixelInLayer == 2)
                    continue;
                if (decodedImage[x][y] != -1)
                    continue;
                decodedImage[x][y] = pixelInLayer;
            }
        }
    }

    for (int y = 0; y < imageHeight; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {
            assert(decodedImage[x][y] == 0 || decodedImage[x][y] == 1);
            cout << (decodedImage[x][y] == 0 ? ' ' : 'X');
        }
        cout << endl;
    }
}
