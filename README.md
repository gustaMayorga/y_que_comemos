# Y Qué Comemos - Meal Planning App

Aplicación completa de planificación de comidas y gestión nutricional familiar para Android e iOS.

## 🎯 Características Principales

### Para Familias
- 📱 **PWA Instalable**: Funciona en Android e iOS como app nativa
- 🏠 **Gestión Familiar**: Perfiles personalizados por edad, necesidades nutricionales e IMC
- 📦 **Inventario Inteligente**: Gestiona alimentos en heladera/alacena con:
  - Entrada manual rápida
  - Escaneo de tickets de compra (OCR)
  - Foto + IA para detectar alimentos automáticamente
- 🍳 **Generador de Recetas**: Crea opciones de comidas basadas en:
  - Alimentos disponibles en casa
  - Preferencias familiares
  - Necesidades nutricionales de cada miembro
  - Horarios y rutinas aprendidas
- 📊 **Planificador Nutricional**:
  - Cálculo de calorías por persona
  - Recomendaciones basadas en IMC
  - Dietas personalizadas
- 🛒 **Lista de Compras Inteligente**: Sugiere qué comprar para completar recetas

### Para Profesionales
- 👨‍⚕️ **Marketplace de Nutricionistas**: Conecta profesionales con usuarios
- 💼 **Consultas Integradas**: Sistema de consultas y seguimiento
- 💰 **Sistema de Pagos**: Fee por transacción para la plataforma

## 🏗️ Arquitectura

### Frontend (PWA)
- **Stack**: React + TypeScript + Vite
- **UI**: TailwindCSS + Shadcn/ui
- **PWA**: Service Workers, offline-first
- **Cámara**: API nativa del navegador
- **Estado**: React Context + Hooks

### Backend (API)
- **Stack**: Node.js + Express + TypeScript
- **Base de Datos**: MongoDB
- **Autenticación**: JWT
- **IA**: OpenAI Vision API
- **OCR**: Google Vision API / Tesseract.js
- **Pagos**: Stripe

## 📁 Estructura del Proyecto

```
y_que_comemos/
├── app/                    # Frontend PWA (React)
│   ├── src/
│   │   ├── components/     # Componentes reutilizables
│   │   ├── pages/          # Páginas de la app
│   │   ├── services/       # Servicios API
│   │   ├── hooks/          # Custom hooks
│   │   ├── context/        # Context providers
│   │   ├── types/          # TypeScript types
│   │   └── utils/          # Utilidades
│   ├── public/
│   └── package.json
│
├── server/                 # Backend API (Node.js)
│   ├── src/
│   │   ├── controllers/    # Controladores
│   │   ├── models/         # Modelos de datos
│   │   ├── routes/         # Rutas API
│   │   ├── middleware/     # Middleware
│   │   ├── services/       # Servicios (IA, OCR, etc)
│   │   └── utils/          # Utilidades
│   └── package.json
│
├── shared/                 # Código compartido
│   └── types/              # TypeScript types compartidos
│
└── package.json            # Root package.json
```

## 🚀 Instalación y Uso

### Requisitos Previos
- Node.js 18+
- MongoDB
- Cuenta en OpenAI (para IA)
- Cuenta en Stripe (para pagos)

### Instalación

```bash
# Clonar repositorio
git clone https://github.com/gustaMayorga/y_que_comemos.git
cd y_que_comemos

# Instalar dependencias
npm install

# Configurar variables de entorno
cp server/.env.example server/.env
# Editar server/.env con tus credenciales

# Iniciar en desarrollo
npm run dev
```

### Variables de Entorno

Crear archivo `server/.env`:

```env
# Server
PORT=3000
NODE_ENV=development

# Database
MONGODB_URI=mongodb://localhost:27017/y_que_comemos

# JWT
JWT_SECRET=tu_secret_key_aqui
JWT_EXPIRE=7d

# OpenAI (para IA)
OPENAI_API_KEY=tu_openai_key

# Stripe (para pagos)
STRIPE_SECRET_KEY=tu_stripe_secret
STRIPE_WEBHOOK_SECRET=tu_webhook_secret

# Google Vision (para OCR) - Opcional
GOOGLE_VISION_API_KEY=tu_google_vision_key
```

## 📱 Instalación como PWA

### Android
1. Abrir la app en Chrome
2. Tocar el menú (⋮) > "Instalar aplicación"
3. Confirmar instalación

### iOS
1. Abrir la app en Safari
2. Tocar el icono de compartir
3. Seleccionar "Agregar a pantalla de inicio"

## 🔧 Desarrollo

```bash
# Desarrollo con hot reload
npm run dev

# Solo frontend
npm run dev:app

# Solo backend
npm run dev:server

# Build producción
npm run build

# Tests
npm run test
```

## 📊 Modelos de Datos

### Usuario
- Información personal
- Tipo (familia / profesional)
- Suscripción
- Preferencias

### Familia
- Miembros (nombre, edad, peso, altura, IMC)
- Restricciones alimentarias
- Preferencias de comida
- Horarios de comida

### Inventario
- Alimentos disponibles
- Cantidad / unidades
- Fecha de vencimiento
- Ubicación (heladera/alacena/freezer)

### Recetas
- Ingredientes necesarios
- Pasos de preparación
- Información nutricional
- Tiempo de preparación
- Dificultad

### Plan de Comidas
- Comidas por día/semana
- Asignación por miembro familiar
- Información nutricional total

## 💰 Modelo de Negocio

### Freemium
- **Gratis**:
  - Gestión básica de inventario
  - Hasta 3 miembros familiares
  - Generación de 5 recetas/semana
  - Calculadora nutricional básica

- **Premium** ($4.99/mes):
  - Inventario ilimitado
  - Miembros ilimitados
  - Recetas ilimitadas con IA
  - Escaneo de tickets
  - Planificación automática semanal/mensual
  - Soporte prioritario

- **Profesional** ($19.99/mes):
  - Todo lo de Premium
  - Perfil en marketplace
  - Gestión de pacientes
  - Analytics y reportes
  - Comisión del 15% por consulta

## 🤝 Contribuir

Las contribuciones son bienvenidas. Por favor:
1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## 📄 Licencia

MIT License - ver archivo LICENSE para más detalles

## 📧 Contacto

Proyecto: [https://github.com/gustaMayorga/y_que_comemos](https://github.com/gustaMayorga/y_que_comemos)
