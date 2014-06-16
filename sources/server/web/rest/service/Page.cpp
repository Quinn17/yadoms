﻿#include "stdafx.h"
#include "Page.h"
#include <shared/exception/NotImplemented.hpp>
#include "web/rest/json/JsonSerializers.h"
#include "web/rest/json/JsonCollectionSerializer.h"
#include "web/rest/RestDispatcherHelpers.hpp"
#include "web/rest/json/JsonResult.h"
#include "Widget.h"

namespace web { namespace rest { namespace service {

   std::string CPage::m_restKeyword= std::string("page");

   CPage::CPage(boost::shared_ptr<database::IDataProvider> dataProvider)
      :m_dataProvider(dataProvider)
   {
   }


   CPage::~CPage()
   {
   }

   const std::string & CPage::getRestKeyword()
   {
      return m_restKeyword;
   }

   void CPage::configureDispatcher(CRestDispatcher & dispatcher)
   {
      REGISTER_DISPATCHER_HANDLER(dispatcher, "GET", (m_restKeyword), CPage::getAllPages);
      REGISTER_DISPATCHER_HANDLER(dispatcher, "GET",  (m_restKeyword)("*"), CPage::getOnePage);
      REGISTER_DISPATCHER_HANDLER(dispatcher, "GET",  (m_restKeyword)("*")(CWidget::getRestKeyword()), CPage::getPageWidget);

      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "POST", (m_restKeyword), CPage::addPage, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "PUT", (m_restKeyword), CPage::updateAllPages, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "PUT", (m_restKeyword)("*"), CPage::updatePage, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "DELETE", (m_restKeyword)("*"), CPage::deletePage, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "DELETE", (m_restKeyword), CPage::deleteAllPages, CPage::transactionalMethod);

      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "POST", (m_restKeyword)("*")(CWidget::getRestKeyword()), CPage::addWidgetForPage, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "PUT", (m_restKeyword)("*")(CWidget::getRestKeyword()), CPage::replaceAllWidgetsForPage, CPage::transactionalMethod);
      REGISTER_DISPATCHER_HANDLER_WITH_INDIRECTOR(dispatcher, "DELETE", (m_restKeyword)("*")(CWidget::getRestKeyword()), CPage::deleteAllWidgetsForPage, CPage::transactionalMethod);
   }


   web::rest::json::CJson CPage::transactionalMethod(CRestDispatcher::CRestMethodHandler realMethod, const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      boost::shared_ptr<database::ITransactionalProvider> pTransactionalEngine = m_dataProvider->getTransactionalEngine();
      web::rest::json::CJson result;
      try
      {
         if(pTransactionalEngine)
            pTransactionalEngine->transactionBegin();
         result = realMethod(parameters, requestContent);
      }
      catch(std::exception &ex)
      {
         result = web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         result = web::rest::json::CJsonResult::GenerateError("unknown exception widget rest method");
      }

      if(pTransactionalEngine)
      {
         if(web::rest::json::CJsonResult::isSuccess(result))
            pTransactionalEngine->transactionCommit();
         else
            pTransactionalEngine->transactionRollback();
      }
      return result;
   }



   web::rest::json::CJson CPage::getOnePage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      if(parameters.size()>1)
      {
         int pageId = boost::lexical_cast<int>(parameters[1]);
         web::rest::json::CPageEntitySerializer hes;
         boost::shared_ptr<database::entities::CPage> pageFound =  m_dataProvider->getPageRequester()->getPage(pageId);
         return web::rest::json::CJsonResult::GenerateSuccess(hes.serialize(*pageFound.get()));
      }
      else
      {
         return web::rest::json::CJsonResult::GenerateError("Invalid parameter count (need page id in url)");
      }
   }

   web::rest::json::CJson CPage::getAllPages(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      web::rest::json::CPageEntitySerializer hes;
      std::vector< boost::shared_ptr<database::entities::CPage> > hwList = m_dataProvider->getPageRequester()->getPages();
      return web::rest::json::CJsonResult::GenerateSuccess(web::rest::json::CJsonCollectionSerializer<database::entities::CPage>::SerializeCollection(hwList, hes, getRestKeyword()));
   }

   web::rest::json::CJson CPage::getPageWidget(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      std::string pageId = "";
      if(parameters.size()>1)
      {
         pageId = parameters[1];

         web::rest::json::CWidgetEntitySerializer hes;
         std::vector< boost::shared_ptr<database::entities::CWidget> > widgetList = m_dataProvider->getWidgetRequester()->getWidgetsForPage(boost::lexical_cast<int>(pageId));
         return web::rest::json::CJsonResult::GenerateSuccess(web::rest::json::CJsonCollectionSerializer<database::entities::CWidget>::SerializeCollection(widgetList, hes, CWidget::getRestKeyword()));
      }
      else
      {
         return web::rest::json::CJsonResult::GenerateError("Invalid parameter count (need page id in url)");
      }
   }

   web::rest::json::CJson CPage::addPage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         web::rest::json::CPageEntitySerializer hes;
         boost::shared_ptr<database::entities::CPage> pageToAdd = hes.deserialize(requestContent);
         int idCreated = m_dataProvider->getPageRequester()->addPage(pageToAdd->Name(), pageToAdd->PageOrder());
         boost::shared_ptr<database::entities::CPage> pageFound =  m_dataProvider->getPageRequester()->getPage(idCreated);
         return web::rest::json::CJsonResult::GenerateSuccess(hes.serialize(*pageFound.get()));
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in creating a new page");
      }
   }

   web::rest::json::CJson CPage::updatePage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         if(parameters.size()>1)
         {
            int pageId = boost::lexical_cast<int>(parameters[1].c_str());

            web::rest::json::CPageEntitySerializer hes;
            boost::shared_ptr<database::entities::CPage> pageToReplace = hes.deserialize(requestContent);
            if(pageToReplace->Id() > 0 && pageId == pageToReplace->Id())
            {
               m_dataProvider->getPageRequester()->updatePage(pageToReplace->Id(), pageToReplace->Name(), pageToReplace->PageOrder());
               return web::rest::json::CJsonResult::GenerateSuccess(hes.serialize(*m_dataProvider->getPageRequester()->getPage(pageToReplace->Id())));
            }
            else
            {
               return web::rest::json::CJsonResult::GenerateError("The page to replace must have a valid id");
            }
         }
         else
         {
            return web::rest::json::CJsonResult::GenerateError("Invalid parameter count (need page id in url)");
         }
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in replacing a page");
      }
   }




   web::rest::json::CJson CPage::updateAllPages(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         m_dataProvider->getPageRequester()->removeAllPages();

         web::rest::json::CPageEntitySerializer hes;
         std::vector<boost::shared_ptr<database::entities::CPage> > pagesToUpdate = web::rest::json::CJsonCollectionSerializer<database::entities::CPage>::DeserializeCollection(requestContent, hes, getRestKeyword());
         BOOST_FOREACH(boost::shared_ptr<database::entities::CPage> page, pagesToUpdate)
         {
            m_dataProvider->getPageRequester()->addPage(*page);
         }

         std::vector<boost::shared_ptr<database::entities::CPage> > allPages = m_dataProvider->getPageRequester()->getPages();
         return web::rest::json::CJsonResult::GenerateSuccess(web::rest::json::CJsonCollectionSerializer<database::entities::CPage>::SerializeCollection(allPages, hes, getRestKeyword()));

      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in updating all pages");
      }
   }





   web::rest::json::CJson CPage::deletePage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         if(parameters.size()>1)
         {
            int pageId = boost::lexical_cast<int>(parameters[1].c_str());

            //delete all widgets
            m_dataProvider->getWidgetRequester()->removeWidgetsInPage(pageId);

            //remove page
            m_dataProvider->getPageRequester()->removePage(pageId);
            return web::rest::json::CJsonResult::GenerateSuccess();
         }
         else
         {
            return web::rest::json::CJsonResult::GenerateError("The page to delete must have an id");
         }
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in deleting a page");
      }
   }

   web::rest::json::CJson CPage::deleteAllPages(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         //delete all widgets
         m_dataProvider->getWidgetRequester()->removeAllWidgets();

         //remove page
         m_dataProvider->getPageRequester()->removeAllPages();
         return web::rest::json::CJsonResult::GenerateSuccess();
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in deleting all pages");
      }
   }

   web::rest::json::CJson CPage::addWidgetForPage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         web::rest::json::CWidgetEntitySerializer hes;
         boost::shared_ptr<database::entities::CWidget> widgetToAdd = hes.deserialize(requestContent);
         int idCreated = m_dataProvider->getWidgetRequester()->addWidget(*widgetToAdd);
         boost::shared_ptr<database::entities::CWidget> widgetFound =  m_dataProvider->getWidgetRequester()->getWidget(idCreated);
         return web::rest::json::CJsonResult::GenerateSuccess(hes.serialize(*widgetFound.get()));
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in creating a new widget");
      }
   }

   web::rest::json::CJson CPage::replaceAllWidgetsForPage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         if(parameters.size()>1)
         {
            int pageId = boost::lexical_cast<int>(parameters[1].c_str());

            //remove all widgets in page
            m_dataProvider->getWidgetRequester()->removeWidgetsInPage(pageId);

            //crreate all
            web::rest::json::CWidgetEntitySerializer hes;
            std::vector<boost::shared_ptr<database::entities::CWidget> > widgetsToAdd = web::rest::json::CJsonCollectionSerializer<database::entities::CWidget>::DeserializeCollection(requestContent, hes, getRestKeyword());
            BOOST_FOREACH(boost::shared_ptr<database::entities::CWidget> pw, widgetsToAdd)
            {
               m_dataProvider->getWidgetRequester()->addWidget(*pw);
            }
            return web::rest::json::CJsonResult::GenerateSuccess();
         }
         else
         {
            return web::rest::json::CJsonResult::GenerateError("Invalid parameter count (need page id in url)");
         }
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in creating a new widget");
      }
   }

   web::rest::json::CJson CPage::deleteAllWidgetsForPage(const std::vector<std::string> & parameters, const web::rest::json::CJson & requestContent)
   {
      try
      {
         if(parameters.size()>1)
         {
            int pageId = boost::lexical_cast<int>(parameters[1].c_str());

            m_dataProvider->getWidgetRequester()->removeWidgetsInPage(pageId);
            return web::rest::json::CJsonResult::GenerateSuccess();
         }
         else
         {
            return web::rest::json::CJsonResult::GenerateError("Invalid parameter count (need page id in url)");
         }
      }
      catch(std::exception &ex)
      {
         return web::rest::json::CJsonResult::GenerateError(ex);
      }
      catch(...)
      {
         return web::rest::json::CJsonResult::GenerateError("unknown exception in creating a new widget");
      }

   }


} //namespace service
} //namespace rest
} //namespace web 