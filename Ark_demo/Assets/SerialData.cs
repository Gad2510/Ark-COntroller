using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class SerialData : MonoBehaviour
{
    public int bottons;
    public float potencia;
    public Vector2 joystic;
    public Vector3 acc;
    
    
    SerialPort dataReader;
    // Start is called before the first frame update
    void Start()
    {
        dataReader = new SerialPort("COM3", 9600);
        dataReader.Open();
        dataReader.ReadTimeout = 50;
        StartCoroutine(ReadInfo());
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    IEnumerator ReadInfo()
    {
        string data= "";
        while (dataReader.IsOpen)
        {
            try
            {
                data=dataReader.ReadLine();
            }
            catch (TimeoutException) { }

            string[]dataV=data.Split('|');
            if (dataV.Length >= 7)
            {
                joystic.x = (float)Math.Round((float.Parse(dataV[0]) - 512) / 512,2)-0.01f;
                joystic.y = (float)Math.Round((float.Parse(dataV[1]) - 512) / 512, 2)-0.01f;

                
                bottons = Convert.ToInt16(dataV[2]);

                acc.x = float.Parse(dataV[3]);
                acc.y = float.Parse(dataV[4]);
                acc.z = float.Parse(dataV[5]);

                potencia = float.Parse(dataV[6]);
            }

            yield return new WaitForSeconds(0.05f);
        }

    }
}
