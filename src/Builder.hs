{-# LANGUAGE DeriveGeneric #-}

module Builder (
    Builder (..), builderKey, builderPath, specified
    ) where

import Base
import Util
import Stage
import Data.List
import Oracles.Base
import Oracles.Setting
import Oracles.WindowsRoot
import GHC.Generics

-- A Builder is an external command invoked in separate process using Shake.cmd
--
-- Ghc Stage0 is the bootstrapping compiler
-- Ghc StageN, N > 0, is the one built on stage (N - 1)
-- GhcPkg Stage0 is the bootstrapping GhcPkg
-- GhcPkg StageN, N > 0, is the one built on stage 0 (TODO: need only Stage1?)
-- TODO: add Cpp builders
-- TODO: rename Gcc to Cc?
data Builder = Ar
             | Ld
             | Alex
             | Happy
             | Haddock
             | HsColour
             | GhcCabal
             | Gcc Stage
             | Ghc Stage
             | GhcM Stage
             | GccM Stage
             | GhcPkg Stage
             | GhcCabalHsColour
             deriving (Show, Eq, Generic)

-- Configuration files refer to Builders as follows:
-- TODO: determine paths to utils without looking up configuration files
builderKey :: Builder -> String
builderKey builder = case builder of
    Ar               -> "ar"
    Ld               -> "ld"
    Alex             -> "alex"
    Happy            -> "happy"
    Haddock          -> "haddock"
    HsColour         -> "hscolour"
    GhcCabal         -> "ghc-cabal"
    Ghc Stage0       -> "system-ghc"
    Ghc Stage1       -> "ghc-stage1"
    Ghc Stage2       -> "ghc-stage2"
    Ghc Stage3       -> "ghc-stage3"
    Gcc Stage0       -> "system-gcc"
    Gcc _            -> "gcc"
    GhcPkg Stage0    -> "system-ghc-pkg"
    GhcPkg _         -> "ghc-pkg"
    -- GhcM/GccM are synonyms for Ghc/Gcc (called with -M and -MM flags)
    GhcM stage       -> builderKey $ Ghc stage
    GccM stage       -> builderKey $ Gcc stage
    -- GhcCabalHsColour is a synonym for GhcCabal (called in hscolour mode)
    GhcCabalHsColour -> builderKey $ GhcCabal

builderPath :: Builder -> Action String
builderPath builder = do
    path <- askConfigWithDefault (builderKey builder) $
            putError $ "\nCannot find path to '" ++ (builderKey builder)
                     ++ "' in configuration files."
    fixAbsolutePathOnWindows $ if null path then "" else path -<.> exe

specified :: Builder -> Action Bool
specified = fmap (not . null) . builderPath

-- On Windows: if the path starts with "/", prepend it with the correct path to
-- the root, e.g: "/usr/local/bin/ghc.exe" => "C:/msys/usr/local/bin/ghc.exe".
fixAbsolutePathOnWindows :: FilePath -> Action FilePath
fixAbsolutePathOnWindows path = do
    windows <- windowsHost
    -- Note, below is different from FilePath.isAbsolute:
    if (windows && "/" `isPrefixOf` path)
    then do
        root <- windowsRoot
        return . unifyPath $ root ++ drop 1 path
    else
        return path

-- When LaxDeps flag is set ('lax-dependencies = YES' in user.config),
-- dependencies on the GHC executable are turned into order-only dependencies
-- to avoid needless recompilation when making changes to GHC's sources. In
-- certain situations this can lead to build failures, in which case you
-- should reset the flag (at least temporarily).

-- Instances for storing in the Shake database
instance Binary Builder
instance Hashable Builder
