#include <iostream>
#include <algorithm>
using namespace std;

int main ()
{
    int n;
    n = 0;
    cin >> n;
    int arr[n];
    if (n==1)
    {
        cin >> arr[0];
        cout << arr[0];
        return 0;
    }
    else if (n==2)
    {
        cin >> arr[0] >> arr[1] ;
        if (arr[0] > arr[1])
        {
            swap(arr[0], arr[1]);
        }
    }
    else 
    {
        cin >> arr[0] >> arr[1] ;
        if (arr[0] > arr[1])
        {
            swap(arr[0],arr[1]);
        }
    
        int j =2;
        int x;
        while (j < n)
        {
            ++j;
            cin >> x;
            for (int k = 0; k < j; ++k)
            {
                if (x < arr[0])
                {
                    for (int l = j; l > 0; --l)
                    {
                        arr[l] = arr[l-1];
                    }
                    arr[0] = x;
                    break;
                }
            
                else if (arr[k] <= x && x <= arr[k+1])
                {
                    for (int l = j; l > k; --l)
                    {
                        arr[l] = arr[l-1];
                    }
                    arr[k+1] = x;
                    break;
                }

                else if (x > arr[j-1])
                {
                    arr[j] = x;
                    break;
                }
            }

            if (j == n)
            {
                break;
            }
        }
    }
    for (int a = 0; a < n; ++a)
    {
        cout << arr[a] << " ";
    }

    return 0;
}