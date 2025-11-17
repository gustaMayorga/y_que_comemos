# Guía de Contribución

¡Gracias por tu interés en contribuir a Y Qué Comemos! Esta guía te ayudará a empezar.

## Código de Conducta

Este proyecto sigue un código de conducta. Al participar, se espera que lo respetes.

## ¿Cómo puedo contribuir?

### Reportar Bugs

Si encuentras un bug:

1. Verifica que no haya sido reportado ya en [Issues](https://github.com/gustaMayorga/y_que_comemos/issues)
2. Si es nuevo, crea un issue con:
   - Título claro y descriptivo
   - Pasos para reproducir el bug
   - Comportamiento esperado vs actual
   - Screenshots si es relevante
   - Información del sistema (OS, navegador, versión de Node)

### Sugerir Mejoras

Para sugerir nuevas características:

1. Abre un issue con el tag `enhancement`
2. Describe claramente la característica
3. Explica por qué sería útil
4. Incluye mockups o ejemplos si es posible

### Pull Requests

1. **Fork el repositorio**

2. **Crea una rama para tu feature:**
   ```bash
   git checkout -b feature/nombre-descriptivo
   ```

3. **Haz tus cambios siguiendo las convenciones de código**

4. **Commit con mensajes descriptivos:**
   ```bash
   git commit -m "feat: Agrega generador de recetas con IA"
   ```

   Convenciones de commits:
   - `feat:` Nueva característica
   - `fix:` Corrección de bug
   - `docs:` Cambios en documentación
   - `style:` Cambios de formato (no afectan código)
   - `refactor:` Refactorización de código
   - `test:` Agregar o modificar tests
   - `chore:` Tareas de mantenimiento

5. **Push a tu fork:**
   ```bash
   git push origin feature/nombre-descriptivo
   ```

6. **Abre un Pull Request:**
   - Título claro y descriptivo
   - Descripción detallada de los cambios
   - Referencias a issues relacionados (#123)
   - Screenshots si aplica

## Desarrollo

### Configuración Inicial

```bash
# Clonar tu fork
git clone https://github.com/TU-USUARIO/y_que_comemos.git
cd y_que_comemos

# Instalar dependencias
npm install

# Configurar variables de entorno
cp server/.env.example server/.env
cp app/.env.example app/.env
# Editar los archivos .env

# Iniciar en desarrollo
npm run dev
```

### Estructura del Proyecto

```
y_que_comemos/
├── app/                    # Frontend React PWA
│   ├── src/
│   │   ├── components/     # Componentes reutilizables
│   │   ├── pages/          # Páginas
│   │   ├── services/       # Servicios API
│   │   ├── hooks/          # Custom hooks
│   │   ├── context/        # React Context
│   │   └── types/          # TypeScript types
├── server/                 # Backend API
│   ├── src/
│   │   ├── controllers/    # Controladores
│   │   ├── models/         # Modelos Mongoose
│   │   ├── routes/         # Rutas Express
│   │   ├── middleware/     # Middleware
│   │   └── services/       # Servicios (IA, OCR)
└── shared/                 # Código compartido
```

### Convenciones de Código

#### TypeScript

- Usa TypeScript para todo el código
- Define tipos explícitos
- Evita `any`, usa `unknown` si es necesario
- Usa interfaces para objetos

```typescript
// Bien ✅
interface User {
  id: string;
  name: string;
  email: string;
}

// Mal ❌
const user: any = {...};
```

#### React

- Componentes funcionales con hooks
- Props tipados
- Nombres de componentes en PascalCase

```typescript
// Bien ✅
interface ButtonProps {
  label: string;
  onClick: () => void;
}

export function Button({ label, onClick }: ButtonProps) {
  return <button onClick={onClick}>{label}</button>;
}
```

#### Naming

- Variables y funciones: `camelCase`
- Componentes: `PascalCase`
- Constantes: `UPPER_CASE`
- Archivos de componentes: `ComponentName.tsx`
- Otros archivos: `fileName.ts`

#### Imports

Ordena imports en este orden:
1. Librerías externas (react, express, etc)
2. Imports internos absolutos
3. Imports relativos
4. Tipos

```typescript
// Bien ✅
import { useState } from 'react';
import { User } from '@/types';
import { formatDate } from '../utils';
import type { UserProps } from './types';
```

### Testing

Actualmente no hay tests implementados. ¡Es una gran área para contribuir!

Si agregas tests:

```bash
# Ejecutar tests
npm test

# Coverage
npm run test:coverage
```

### Linting y Formatting

```bash
# Linter
npm run lint

# Fix automático
npm run lint:fix
```

## Áreas de Contribución

### 🌟 Alta Prioridad

- [ ] Implementar tests (Jest + React Testing Library)
- [ ] Mejorar sistema de generación de recetas con IA
- [ ] Optimizar OCR para escaneo de tickets
- [ ] Implementar notificaciones push
- [ ] Agregar soporte offline (Service Workers)

### 🚀 Features Nuevas

- [ ] Integración con smart watches
- [ ] Generación de listas de compras inteligentes
- [ ] Recomendaciones basadas en temporada
- [ ] Recetas en video
- [ ] Chat con nutricionistas
- [ ] Calculadora de porciones
- [ ] Modo oscuro

### 🐛 Bug Fixes

Revisa los [issues abiertos](https://github.com/gustaMayorga/y_que_comemos/issues) con el tag `bug`

### 📖 Documentación

- Mejorar guías existentes
- Agregar tutoriales
- Traducir a otros idiomas
- Crear videos tutoriales

## Revisión de Pull Requests

Los PRs serán revisados considerando:

1. **Funcionalidad**: ¿Funciona correctamente?
2. **Código**: ¿Sigue las convenciones?
3. **Tests**: ¿Incluye tests? (cuando aplique)
4. **Documentación**: ¿Está documentado?
5. **Performance**: ¿Afecta el rendimiento?

## Preguntas

Si tienes dudas:

1. Revisa la documentación existente
2. Busca en issues cerrados
3. Abre un issue con el tag `question`

## Licencia

Al contribuir, aceptas que tus contribuciones serán licenciadas bajo la licencia MIT del proyecto.

## Agradecimientos

¡Gracias por contribuir a Y Qué Comemos! 🎉🍽️

Tu ayuda hace que esta app sea mejor para todos.
