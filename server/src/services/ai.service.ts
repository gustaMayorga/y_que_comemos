import OpenAI from 'openai';
import fs from 'fs';
import { FoodCategory, FoodLocation } from '../models/FoodItem.model.js';
import { RecipeDifficulty } from '../models/Recipe.model.js';

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

interface RecipeGenerationParams {
  availableIngredients: string[];
  servings: number;
  difficulty?: string;
  mealType?: string;
  dietaryRestrictions: string[];
  allergies: string[];
  maxTime: number;
}

interface GeneratedRecipe {
  name: string;
  description: string;
  difficulty: RecipeDifficulty;
  prepTime: number;
  cookTime: number;
  servings: number;
  ingredients: Array<{
    foodItem: string;
    quantity: number;
    unit: string;
  }>;
  instructions: string[];
  nutritionalInfo: {
    calories: number;
    protein: number;
    carbs: number;
    fat: number;
    fiber: number;
  };
  tags: string[];
}

export async function generateRecipesWithAI(
  params: RecipeGenerationParams
): Promise<GeneratedRecipe[]> {
  try {
    const prompt = `
Eres un chef experto y nutricionista. Genera 3 recetas saludables y deliciosas con las siguientes especificaciones:

**Ingredientes disponibles**: ${params.availableIngredients.join(', ') || 'Cualquier ingrediente común'}
**Porciones**: ${params.servings}
**Dificultad**: ${params.difficulty || 'cualquiera'}
**Tipo de comida**: ${params.mealType || 'cualquiera'}
**Restricciones dietéticas**: ${params.dietaryRestrictions.join(', ') || 'ninguna'}
**Alergias**: ${params.allergies.join(', ') || 'ninguna'}
**Tiempo máximo de preparación**: ${params.maxTime} minutos

Para cada receta, proporciona:
1. Nombre atractivo
2. Descripción breve (1-2 líneas)
3. Dificultad (easy, medium, hard)
4. Tiempo de preparación en minutos
5. Tiempo de cocción en minutos
6. Lista de ingredientes con cantidades exactas y unidades
7. Instrucciones paso a paso
8. Información nutricional por porción (calorías, proteínas, carbohidratos, grasas, fibra en gramos)
9. Tags relevantes (ejemplo: vegetariano, bajo en calorías, rico en proteínas, etc.)

Responde SOLO con un JSON válido en este formato:
{
  "recipes": [
    {
      "name": "Nombre de la receta",
      "description": "Descripción breve",
      "difficulty": "easy|medium|hard",
      "prepTime": 15,
      "cookTime": 30,
      "servings": 4,
      "ingredients": [
        {"foodItem": "Pollo", "quantity": 500, "unit": "g"},
        {"foodItem": "Arroz", "quantity": 2, "unit": "tazas"}
      ],
      "instructions": [
        "Paso 1: ...",
        "Paso 2: ..."
      ],
      "nutritionalInfo": {
        "calories": 450,
        "protein": 35,
        "carbs": 40,
        "fat": 15,
        "fiber": 5
      },
      "tags": ["saludable", "alto en proteínas"]
    }
  ]
}
`;

    const completion = await openai.chat.completions.create({
      model: 'gpt-4',
      messages: [
        {
          role: 'system',
          content:
            'Eres un chef experto y nutricionista que genera recetas saludables y deliciosas. Respondes solo con JSON válido.',
        },
        {
          role: 'user',
          content: prompt,
        },
      ],
      temperature: 0.8,
      max_tokens: 2000,
    });

    const response = completion.choices[0]?.message?.content;
    if (!response) {
      throw new Error('No se recibió respuesta de OpenAI');
    }

    // Parse JSON response
    const jsonMatch = response.match(/\{[\s\S]*\}/);
    if (!jsonMatch) {
      throw new Error('No se pudo extraer JSON de la respuesta');
    }

    const data = JSON.parse(jsonMatch[0]);
    return data.recipes || [];
  } catch (error) {
    console.error('Error generating recipes with AI:', error);

    // Fallback recipes if AI fails
    return getFallbackRecipes(params);
  }
}

export async function detectFoodItems(imagePath: string): Promise<any[]> {
  try {
    // Read image file
    const imageBuffer = fs.readFileSync(imagePath);
    const base64Image = imageBuffer.toString('base64');

    const response = await openai.chat.completions.create({
      model: 'gpt-4-vision-preview',
      messages: [
        {
          role: 'user',
          content: [
            {
              type: 'text',
              text: `Analiza esta imagen e identifica todos los alimentos visibles.
Para cada alimento, proporciona:
- Nombre del alimento
- Categoría (fruits, vegetables, proteins, dairy, grains, snacks, beverages, condiments, other)
- Cantidad estimada (número)
- Unidad (kg, g, L, ml, units)
- Ubicación recomendada (fridge, pantry, freezer)

Responde SOLO con un JSON válido en este formato:
{
  "items": [
    {
      "name": "Manzana",
      "category": "fruits",
      "quantity": 5,
      "unit": "units",
      "location": "fridge"
    }
  ]
}`,
            },
            {
              type: 'image_url',
              image_url: {
                url: `data:image/jpeg;base64,${base64Image}`,
              },
            },
          ],
        },
      ],
      max_tokens: 1000,
    });

    const content = response.choices[0]?.message?.content;
    if (!content) {
      throw new Error('No se recibió respuesta de OpenAI Vision');
    }

    // Parse JSON response
    const jsonMatch = content.match(/\{[\s\S]*\}/);
    if (!jsonMatch) {
      throw new Error('No se pudo extraer JSON de la respuesta');
    }

    const data = JSON.parse(jsonMatch[0]);

    // Add purchase date to each item
    return data.items.map((item: any) => ({
      ...item,
      purchaseDate: new Date(),
    }));
  } catch (error) {
    console.error('Error detecting food items:', error);
    return [];
  }
}

function getFallbackRecipes(params: RecipeGenerationParams): GeneratedRecipe[] {
  // Fallback recipes when AI is not available
  return [
    {
      name: 'Ensalada César con Pollo',
      description: 'Una clásica ensalada César saludable con pollo a la parrilla',
      difficulty: RecipeDifficulty.EASY,
      prepTime: 15,
      cookTime: 15,
      servings: params.servings,
      ingredients: [
        { foodItem: 'Lechuga romana', quantity: 1, unit: 'unidad' },
        { foodItem: 'Pechuga de pollo', quantity: 400, unit: 'g' },
        { foodItem: 'Queso parmesano', quantity: 50, unit: 'g' },
        { foodItem: 'Aceite de oliva', quantity: 2, unit: 'cucharadas' },
      ],
      instructions: [
        'Cocina el pollo a la parrilla hasta que esté bien cocido',
        'Lava y corta la lechuga',
        'Mezcla todos los ingredientes',
        'Sirve inmediatamente',
      ],
      nutritionalInfo: {
        calories: 350,
        protein: 35,
        carbs: 15,
        fat: 18,
        fiber: 3,
      },
      tags: ['saludable', 'alto en proteínas', 'bajo en carbohidratos'],
    },
  ];
}
