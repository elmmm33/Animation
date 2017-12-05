#version 120
varying vec4 normal;
varying vec4 pos;

void main (void)  
{  
    vec4 color = vec4(0, 0, 0, 0);

    for (int i = 0; i < 2; ++i)
    {
        vec4 lightDir = normalize(gl_LightSource[i].position);
        float NdotL = max(dot(normal, lightDir), 0.0);
        float dist = distance(gl_LightSource[i].position, pos);

        float distAtten = 0.3 / (0.25 + 0.0033 * dist + 0.000045492 * dist * dist);
        
        vec4 L = normalize(gl_LightSource[i].position - pos);
        vec4 R = normalize(-reflect(L, normal));
        vec4 E = normalize(-pos);
        vec4 Ispec = gl_FrontLightProduct[0].specular * pow(max(dot(R,E), 0.0), 0.3 * gl_FrontMaterial.shininess);
        Ispec = clamp(Ispec, 0.0, 1.0);
        
        color = color + NdotL * gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * distAtten + Ispec;
    }
    
    color = color + gl_FrontMaterial.ambient;
    int refid = 0;
    if (color[1] >= color[0] && color[1] >= color[0])
        refid = 1;
    else if (color[2] >= color[0] && color[2] >= color[1])
        refid = 2;
        
    float colref = color[refid];
    int colrefi = int(colref * 100);
    color[refid] = colrefi / 20 / 5.0;
    color[(refid + 1) - (refid + 1)/3*3] = color[(refid + 1) - (refid + 1)/3*3] * color[refid] / colref;
    color[(refid + 2) - (refid + 2)/3*3] = color[(refid + 2) - (refid + 2)/3*3] * color[refid] / colref;
    
    gl_FragColor = color;
}
