# Guía de Instalación - Y Qué Comemos

Esta guía te ayudará a configurar y ejecutar la aplicación en tu entorno local.

## Requisitos Previos

Asegúrate de tener instalado:

- **Node.js** 18 o superior ([Descargar](https://nodejs.org/))
- **MongoDB** 5.0 o superior ([Descargar](https://www.mongodb.com/try/download/community))
- **npm** o **yarn** (viene con Node.js)
- **Git** ([Descargar](https://git-scm.com/))

## Instalación Paso a Paso

### 1. Clonar el Repositorio

```bash
git clone https://github.com/gustaMayorga/y_que_comemos.git
cd y_que_comemos
```

### 2. Instalar Dependencias

Instala las dependencias del proyecto root, frontend y backend:

```bash
# Instalar todas las dependencias (root + workspaces)
npm install

# O instalar individualmente
cd app && npm install
cd ../server && npm install
```

### 3. Configurar MongoDB

#### Opción A: MongoDB Local

1. Inicia MongoDB:

```bash
# En Linux/Mac
sudo systemctl start mongod

# En Windows
# Inicia el servicio MongoDB desde Servicios
```

2. Verifica que MongoDB esté corriendo:

```bash
mongosh
# Deberías ver el prompt de MongoDB
```

#### Opción B: MongoDB Atlas (Cloud)

1. Crea una cuenta gratuita en [MongoDB Atlas](https://www.mongodb.com/cloud/atlas)
2. Crea un cluster gratuito
3. Obtén tu cadena de conexión (connection string)
4. Agrega tu IP a la whitelist

### 4. Configurar Variables de Entorno

#### Backend (.env)

Crea el archivo `server/.env` basado en `server/.env.example`:

```bash
cd server
cp .env.example .env
```

Edita `server/.env` con tus credenciales:

```env
# Server
PORT=3000
NODE_ENV=development

# Database
MONGODB_URI=mongodb://localhost:27017/y_que_comemos
# O si usas MongoDB Atlas:
# MONGODB_URI=mongodb+srv://user:password@cluster.mongodb.net/y_que_comemos

# JWT
JWT_SECRET=genera_un_secret_seguro_aqui_con_al_menos_32_caracteres
JWT_EXPIRE=7d

# OpenAI (IMPORTANTE para IA)
OPENAI_API_KEY=sk-tu_api_key_aqui

# Stripe (opcional para desarrollo)
STRIPE_SECRET_KEY=sk_test_tu_stripe_key
STRIPE_WEBHOOK_SECRET=whsec_tu_webhook_secret

# CORS
CORS_ORIGIN=http://localhost:5173
```

**Obtener API Keys:**

- **OpenAI**: Regístrate en [OpenAI Platform](https://platform.openai.com/) y genera una API key
- **Stripe** (opcional): Regístrate en [Stripe](https://stripe.com/) para pruebas

#### Frontend (.env)

Crea el archivo `app/.env`:

```bash
cd app
cp .env.example .env
```

Contenido de `app/.env`:

```env
VITE_API_URL=http://localhost:3000/api
VITE_APP_NAME=Y Qué Comemos
VITE_APP_VERSION=1.0.0
```

### 5. Iniciar la Aplicación

#### Opción A: Iniciar Todo con un Comando

Desde la raíz del proyecto:

```bash
npm run dev
```

Esto iniciará:
- Frontend en `http://localhost:5173`
- Backend en `http://localhost:3000`

#### Opción B: Iniciar por Separado

**Terminal 1 - Backend:**
```bash
cd server
npm run dev
```

**Terminal 2 - Frontend:**
```bash
cd app
npm run dev
```

### 6. Verificar la Instalación

1. Abre tu navegador en `http://localhost:5173`
2. Deberías ver la página principal de "Y Qué Comemos"
3. Crea una cuenta nueva para probar la aplicación

#### Verificar Backend

Visita `http://localhost:3000/api/health` - deberías ver:

```json
{
  "status": "ok",
  "timestamp": "2024-01-..."
}
```

## Solución de Problemas Comunes

### Error: MongoDB connection failed

**Causa**: MongoDB no está corriendo o la URI es incorrecta

**Solución**:
```bash
# Verifica que MongoDB esté corriendo
sudo systemctl status mongod

# O inicia MongoDB
sudo systemctl start mongod

# Verifica la conexión
mongosh
```

### Error: OpenAI API key invalid

**Causa**: La API key de OpenAI no es válida

**Solución**:
1. Verifica que tu API key esté correcta en `server/.env`
2. Asegúrate de tener créditos en tu cuenta de OpenAI
3. La key debe empezar con `sk-`

### Error: Port already in use

**Causa**: El puerto 3000 o 5173 ya está siendo usado

**Solución**:
```bash
# Encuentra el proceso usando el puerto
# Linux/Mac
lsof -i :3000
kill -9 <PID>

# Windows
netstat -ano | findstr :3000
taskkill /PID <PID> /F

# O cambia el puerto en las variables de entorno
```

### Error: Module not found

**Causa**: Las dependencias no se instalaron correctamente

**Solución**:
```bash
# Elimina node_modules y reinstala
rm -rf node_modules app/node_modules server/node_modules
rm package-lock.json app/package-lock.json server/package-lock.json
npm install
```

## Datos de Prueba

Para facilitar las pruebas, puedes crear usuarios de prueba manualmente o usar la API:

### Crear Usuario Vía API

```bash
curl -X POST http://localhost:3000/api/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "email": "test@example.com",
    "password": "password123",
    "name": "Usuario de Prueba"
  }'
```

## Siguiente Paso: Uso de la Aplicación

Una vez instalada, consulta la [Guía de Usuario](./USER_GUIDE.md) para aprender a usar todas las características de la aplicación.

## Desarrollo

### Scripts Disponibles

```bash
# Root
npm run dev           # Inicia frontend + backend
npm run dev:app       # Solo frontend
npm run dev:server    # Solo backend
npm run build         # Build producción
npm test             # Ejecutar tests

# Frontend (app/)
npm run dev          # Desarrollo con hot reload
npm run build        # Build para producción
npm run preview      # Preview del build

# Backend (server/)
npm run dev          # Desarrollo con hot reload
npm run build        # Compilar TypeScript
npm run start        # Ejecutar versión compilada
```

### Estructura del Proyecto

```
y_que_comemos/
├── app/                    # Frontend React PWA
│   ├── src/
│   │   ├── components/     # Componentes reutilizables
│   │   ├── pages/          # Páginas de la app
│   │   ├── services/       # Servicios API
│   │   └── types/          # TypeScript types
│   └── package.json
├── server/                 # Backend API
│   ├── src/
│   │   ├── controllers/    # Controladores
│   │   ├── models/         # Modelos MongoDB
│   │   ├── routes/         # Rutas API
│   │   ├── middleware/     # Middleware
│   │   └── services/       # Servicios (IA, OCR)
│   └── package.json
└── package.json            # Root package.json
```

## Soporte

Si tienes problemas con la instalación:

1. Revisa esta guía completa
2. Consulta la sección de problemas comunes
3. Abre un issue en [GitHub](https://github.com/gustaMayorga/y_que_comemos/issues)

## Licencia

MIT License - ver archivo [LICENSE](./LICENSE) para más detalles
