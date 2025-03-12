using System.Runtime.InteropServices;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Subscriber : MonoBehaviour
{
    // Importar las funciones desde la DLL
    [DllImport("DLL")]
    private static extern void SetTransformPosition(float x, float y, float z);
    
    [DllImport("DLL")]
    private static extern float GetPositionX();
    
    [DllImport("DLL")]
    private static extern float GetPositionY();
    
    [DllImport("DLL")]
    private static extern float GetPositionZ();
    
    [DllImport("DLL")]
    private static extern float incrementXPosition();
    
    [DllImport("DLL")]
    private static extern float incrementYPosition();
    
    [DllImport("DLL")]
    private static extern float incrementZPosition();
    
    [DllImport("DLL")]
    private static extern void saveLog();
    
    public GameObject Target;
    private float timer = 0f;
    private float interval = 1f;
        
    void Start()
    {
    	
    	 if (Target == null)
        {
            Debug.LogError("El objeto Target no está asignado en el Inspector.");
            return;
        }
        Vector3 initialPosition = Target.transform.position;
        SetTransformPosition(initialPosition.x, initialPosition.y, initialPosition.z);
        Debug.Log($"Posición inicial de Target sincronizada con la DLL: ({initialPosition.x}, {initialPosition.y}, {initialPosition.z})");
    }
    
    void Update()
    {
        if (Target != null)
        {
       	   timer += Time.deltaTime;
           if (timer >= interval)
        {
            timer = 0f;
            float positionXNew = GetPositionX();
            float positionYNew = GetPositionY();
            float positionZNew = GetPositionZ();
            
            Vector3 currentPosition = Target.transform.position;
            
            Target.transform.position = new Vector3(positionXNew, positionYNew, positionZNew);
            
            SetTransformPosition(positionXNew, positionYNew, positionZNew);
        }
        }
        else
        {
            Debug.LogWarning("El objeto Target no está asignado.");
        }
    }
    void OnApplicationQuit()
    {
    	Debug.Log("Cerrando app");
    	saveLog();
    }

}
