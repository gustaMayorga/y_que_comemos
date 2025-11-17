# Documentación de API - Y Qué Comemos

API REST para la aplicación de planificación de comidas.

**Base URL**: `http://localhost:3000/api`

## Autenticación

La mayoría de los endpoints requieren autenticación mediante JWT (JSON Web Token).

### Headers Requeridos

```
Authorization: Bearer <token>
Content-Type: application/json
```

## Endpoints

### 🔐 Autenticación

#### POST /auth/register

Registra un nuevo usuario.

**Request Body:**
```json
{
  "email": "user@example.com",
  "password": "password123",
  "name": "Juan Pérez"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "user": {
      "_id": "...",
      "email": "user@example.com",
      "name": "Juan Pérez",
      "role": "family",
      "subscription": "free"
    },
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
  }
}
```

#### POST /auth/login

Inicia sesión.

**Request Body:**
```json
{
  "email": "user@example.com",
  "password": "password123"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "user": { ... },
    "token": "..."
  }
}
```

#### GET /auth/me

Obtiene el perfil del usuario autenticado.

**Headers:** `Authorization: Bearer <token>`

**Response:**
```json
{
  "success": true,
  "data": {
    "_id": "...",
    "email": "user@example.com",
    "name": "Juan Pérez",
    ...
  }
}
```

#### PUT /auth/profile

Actualiza el perfil del usuario.

**Request Body:**
```json
{
  "name": "Juan Pérez Actualizado",
  "avatar": "https://..."
}
```

---

### 📦 Inventario

#### GET /inventory

Obtiene el inventario del usuario.

**Query Parameters:**
- `category` (opcional): Filtrar por categoría
- `location` (opcional): Filtrar por ubicación
- `search` (opcional): Buscar por nombre

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "_id": "...",
      "name": "Tomates",
      "category": "vegetables",
      "quantity": 5,
      "unit": "units",
      "location": "fridge",
      "expirationDate": "2024-02-15",
      ...
    }
  ]
}
```

#### POST /inventory

Agrega un item al inventario.

**Request Body:**
```json
{
  "name": "Tomates",
  "category": "vegetables",
  "quantity": 5,
  "unit": "units",
  "location": "fridge",
  "expirationDate": "2024-02-15"
}
```

#### PUT /inventory/:id

Actualiza un item del inventario.

#### DELETE /inventory/:id

Elimina un item del inventario.

#### POST /inventory/scan-receipt

Escanea un ticket de compra (OCR).

**Request:** `multipart/form-data`
- `image`: Archivo de imagen del ticket

**Response:**
```json
{
  "success": true,
  "data": {
    "text": "...",
    "message": "Ticket procesado"
  }
}
```

#### POST /inventory/detect-food

Detecta alimentos en una foto usando IA.

**Request:** `multipart/form-data`
- `image`: Archivo de imagen con alimentos

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "name": "Manzana",
      "category": "fruits",
      "quantity": 3,
      ...
    }
  ],
  "message": "3 alimentos detectados"
}
```

---

### 🍳 Recetas

#### GET /recipes

Obtiene las recetas del usuario.

**Query Parameters:**
- `difficulty` (opcional): easy, medium, hard
- `search` (opcional): Buscar por nombre
- `tags` (opcional): Filtrar por tags (separados por coma)

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "_id": "...",
      "name": "Pasta Carbonara",
      "description": "Deliciosa pasta italiana",
      "difficulty": "medium",
      "prepTime": 10,
      "cookTime": 20,
      "servings": 4,
      "ingredients": [...],
      "instructions": [...],
      "nutritionalInfo": {...},
      "tags": ["italiana", "pasta"],
      ...
    }
  ]
}
```

#### GET /recipes/:id

Obtiene una receta específica.

#### POST /recipes

Crea una receta manualmente.

**Request Body:**
```json
{
  "name": "Pasta Carbonara",
  "description": "Deliciosa pasta italiana",
  "difficulty": "medium",
  "prepTime": 10,
  "cookTime": 20,
  "servings": 4,
  "ingredients": [
    {
      "foodItem": "Pasta",
      "quantity": 400,
      "unit": "g"
    },
    {
      "foodItem": "Huevos",
      "quantity": 3,
      "unit": "units"
    }
  ],
  "instructions": [
    "Cocinar la pasta",
    "Preparar la salsa",
    "Mezclar todo"
  ],
  "nutritionalInfo": {
    "calories": 450,
    "protein": 18,
    "carbs": 55,
    "fat": 15,
    "fiber": 3
  },
  "tags": ["italiana", "pasta"]
}
```

#### PUT /recipes/:id

Actualiza una receta.

#### DELETE /recipes/:id

Elimina una receta.

#### POST /recipes/generate

Genera recetas con IA basadas en inventario y preferencias.

**Request Body:**
```json
{
  "servings": 4,
  "difficulty": "easy",
  "mealType": "lunch",
  "dietaryRestrictions": ["vegetariano"],
  "maxTime": 60,
  "useInventory": true
}
```

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "name": "Ensalada Mediterránea",
      "description": "Fresca y saludable",
      ...
    }
  ],
  "message": "3 recetas generadas exitosamente"
}
```

---

### 📅 Plan de Comidas

#### GET /meal-plans

Obtiene todos los planes de comida del usuario.

#### GET /meal-plans/:id

Obtiene un plan específico.

#### POST /meal-plans

Crea un nuevo plan de comidas.

**Request Body:**
```json
{
  "startDate": "2024-02-01",
  "endDate": "2024-02-07",
  "meals": [
    {
      "date": "2024-02-01",
      "mealType": "breakfast",
      "recipeId": "...",
      "memberIds": ["member1", "member2"],
      "notes": "Sin azúcar para María"
    }
  ],
  "shoppingList": [
    {
      "foodItem": "Leche",
      "quantity": 2,
      "unit": "L",
      "category": "dairy",
      "purchased": false
    }
  ]
}
```

#### PUT /meal-plans/:id

Actualiza un plan de comidas.

#### DELETE /meal-plans/:id

Elimina un plan de comidas.

#### POST /meal-plans/generate-weekly

Genera automáticamente un plan semanal (TODO).

---

### 👨‍👩‍👧‍👦 Familia

#### GET /family

Obtiene el perfil familiar del usuario.

**Response:**
```json
{
  "success": true,
  "data": {
    "_id": "...",
    "members": [
      {
        "_id": "...",
        "name": "Juan",
        "age": 35,
        "weight": 75,
        "height": 175,
        "gender": "male",
        "dietaryRestrictions": ["bajo en sodio"],
        "allergies": ["nueces"],
        "bmi": 24.5,
        "dailyCalories": 2200
      }
    ],
    "mealSchedule": {
      "breakfast": "08:00",
      "lunch": "13:00",
      "dinner": "20:00"
    },
    "preferences": ["cocina casera", "bajo en grasas"]
  }
}
```

#### POST /family

Crea el perfil familiar.

#### PUT /family

Actualiza el perfil familiar.

#### POST /family/members

Agrega un miembro a la familia.

**Request Body:**
```json
{
  "name": "María",
  "age": 8,
  "weight": 30,
  "height": 130,
  "gender": "female",
  "dietaryRestrictions": [],
  "allergies": ["lactosa"]
}
```

#### PUT /family/members/:memberId

Actualiza un miembro.

#### DELETE /family/members/:memberId

Elimina un miembro.

---

### 👨‍⚕️ Profesionales

#### GET /professionals

Obtiene la lista de nutricionistas (público).

**Query Parameters:**
- `specialization` (opcional): Filtrar por especialización
- `minRating` (opcional): Rating mínimo
- `search` (opcional): Buscar por nombre/bio

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "_id": "...",
      "userId": {...},
      "specialization": ["Pérdida de peso", "Diabetes"],
      "bio": "Nutricionista certificado...",
      "experience": 8,
      "rating": 4.8,
      "reviewCount": 127,
      "hourlyRate": 45,
      "availability": [...],
      "certifications": [...]
    }
  ]
}
```

#### GET /professionals/:id

Obtiene un profesional específico.

#### POST /professionals

Crea perfil profesional (requiere role: professional).

#### PUT /professionals

Actualiza perfil profesional.

---

## Códigos de Estado HTTP

- `200 OK`: Solicitud exitosa
- `201 Created`: Recurso creado exitosamente
- `400 Bad Request`: Datos inválidos
- `401 Unauthorized`: No autenticado
- `403 Forbidden`: Sin permisos
- `404 Not Found`: Recurso no encontrado
- `500 Internal Server Error`: Error del servidor

## Formato de Error

Todos los errores siguen este formato:

```json
{
  "success": false,
  "message": "Descripción del error"
}
```

## Rate Limiting

(TODO: Implementar rate limiting en producción)

## Websockets

(TODO: Implementar notificaciones en tiempo real)

## Paginación

Los endpoints que devuelven listas soportarán paginación en versiones futuras:

```
GET /recipes?page=1&limit=20
```

## Versionado

Versión actual: `v1`

Todas las rutas están bajo `/api/`

## Postman Collection

Puedes importar la colección de Postman para probar todos los endpoints:

[Descargar Postman Collection](./postman_collection.json) (TODO)
